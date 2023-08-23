#pragma once

#include "../pch.hpp"
#include "device.hpp"

namespace fb {

enum class GpuTextureFlags : uint32_t {
    None = 0x0,
    Srv = 0x1,
    Uav = 0x2,
    Rtv = 0x4,
    Dsv = 0x8,
    Cube = 0x10,
    SrvUav = Srv | Uav,
    SrvRtv = Srv | Rtv,
    SrvDsv = Srv | Dsv,
    SrvCube = Srv | Cube,
    SrvUavRtv = Srv | Uav | Rtv,
};

inline constexpr auto gpu_texture_flags_contains(GpuTextureFlags flags, GpuTextureFlags flag)
    -> bool {
    return ((uint32_t)flags & (uint32_t)flag) == (uint32_t)flag;
}

struct GpuTextureDesc {
    DXGI_FORMAT format = DXGI_FORMAT_UNKNOWN;
    uint32_t width = 1;
    uint32_t height = 1;
    uint32_t depth = 1;
    uint32_t mip_count = 1;
    uint32_t sample_count = 1;
    std::optional<D3D12_CLEAR_VALUE> clear_value = std::nullopt;
    std::optional<DXGI_FORMAT> srv_format = std::nullopt;
    std::optional<DXGI_FORMAT> uav_format = std::nullopt;
    std::optional<DXGI_FORMAT> rtv_format = std::nullopt;
    std::optional<DXGI_FORMAT> dsv_format = std::nullopt;
};

struct GpuTextureTransferDesc {
    const void* data = nullptr;
    uint32_t row_pitch = 0;
    uint32_t slice_pitch = 0;
};

template<GpuTextureFlags FLAGS>
class GpuTexture {
    FB_NO_COPY_MOVE(GpuTexture);

public:
    using enum GpuTextureFlags;

    static inline constexpr auto MAX_MIP_COUNT = 16;

    GpuTexture() = default;

    auto create(GpuDevice& device, const GpuTextureDesc& desc, std::string_view name) -> void {
        // Validate.
        FB_ASSERT(desc.format != DXGI_FORMAT_UNKNOWN);
        FB_ASSERT(desc.width > 0);
        FB_ASSERT(desc.height > 0);
        FB_ASSERT(desc.depth > 0);
        FB_ASSERT(desc.mip_count > 0 && desc.mip_count <= MAX_MIP_COUNT);
        FB_ASSERT(desc.sample_count > 0);
        if (gpu_texture_flags_contains(FLAGS, Cube)) {
            FB_ASSERT(desc.depth == 6);
        }

        // Optionally override view formats.
        _srv_format = desc.srv_format.value_or(desc.format);
        _uav_format = desc.uav_format.value_or(desc.format);
        _rtv_format = desc.rtv_format.value_or(desc.format);
        _dsv_format = desc.dsv_format.value_or(desc.format);

        // Heap properties.
        const auto heap_props = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);

        // Texture flags.
        D3D12_RESOURCE_FLAGS resource_flags = D3D12_RESOURCE_FLAG_NONE;
        D3D12_RESOURCE_STATES init_state = D3D12_RESOURCE_STATE_COMMON;
        if (gpu_texture_flags_contains(FLAGS, Uav)) {
            resource_flags |= D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;
        }
        if (gpu_texture_flags_contains(FLAGS, Rtv)) {
            resource_flags |= D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;
            init_state = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
        }
        if (gpu_texture_flags_contains(FLAGS, Dsv)) {
            resource_flags |= D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;
            init_state = D3D12_RESOURCE_STATE_DEPTH_WRITE;
        }

        // Resource desc.
        D3D12_RESOURCE_DESC resource_desc {
            .Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D,
            .Alignment = 0,
            .Width = (uint64_t)desc.width,
            .Height = desc.height,
            .DepthOrArraySize = (uint16_t)desc.depth,
            .MipLevels = (uint16_t)desc.mip_count,
            .Format = desc.format,
            .SampleDesc = DXGI_SAMPLE_DESC {.Count = desc.sample_count, .Quality = 0},
            .Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN,
            .Flags = resource_flags,
        };

        // Clear value.
        std::optional<D3D12_CLEAR_VALUE> clear_value = std::nullopt;
        if (gpu_texture_flags_contains(FLAGS, Rtv)) {
            FB_ASSERT(desc.clear_value.has_value());
            clear_value = desc.clear_value;
        } else if (gpu_texture_flags_contains(FLAGS, Dsv)) {
            FB_ASSERT(desc.clear_value.has_value());
            clear_value = desc.clear_value;
        } else {
            FB_ASSERT(!desc.clear_value.has_value());
        }

        // Resource.
        _resource = device.create_committed_resource(
            heap_props,
            resource_desc,
            init_state,
            clear_value,
            name
        );
        _resource_state = init_state;

        // Views.
        if constexpr (gpu_texture_flags_contains(FLAGS, Srv)) {
            _srv_descriptor = device.descriptors().cbv_srv_uav().alloc();

            D3D12_SRV_DIMENSION srv_dimension;
            if (gpu_texture_flags_contains(FLAGS, Cube)) {
                srv_dimension = D3D12_SRV_DIMENSION_TEXTURECUBE;
            } else if (desc.sample_count > 1) {
                srv_dimension = D3D12_SRV_DIMENSION_TEXTURE2DMS;
            } else {
                srv_dimension = D3D12_SRV_DIMENSION_TEXTURE2D;
            }

            device.create_shader_resource_view(
                _resource,
                D3D12_SHADER_RESOURCE_VIEW_DESC {
                    .Format = _srv_format,
                    .ViewDimension = srv_dimension,
                    .Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING,
                    .Texture2D = D3D12_TEX2D_SRV {.MipLevels = desc.mip_count},
                },
                _srv_descriptor.cpu()
            );
        }
        if constexpr (gpu_texture_flags_contains(FLAGS, Uav)) {
            static_assert(!gpu_texture_flags_contains(FLAGS, Cube));
            for (uint32_t mip = 0; mip < desc.mip_count; mip++) {
                _uav_descriptors[mip] = device.descriptors().cbv_srv_uav().alloc();
                device.create_unordered_access_view(
                    _resource,
                    std::nullopt,
                    D3D12_UNORDERED_ACCESS_VIEW_DESC {
                        .Format = _uav_format,
                        .ViewDimension = D3D12_UAV_DIMENSION_TEXTURE2D,
                        .Texture2D = {.MipSlice = mip, .PlaneSlice = 0},
                    },
                    _uav_descriptors[mip].cpu()
                );
            }
        }
        if constexpr (gpu_texture_flags_contains(FLAGS, Rtv)) {
            _rtv_descriptor = device.descriptors().rtv().alloc();
            std::optional<D3D12_RENDER_TARGET_VIEW_DESC> maybe_desc = std::nullopt;
            if (D3D12_PROPERTY_LAYOUT_FORMAT_TABLE::GetTypeLevel(desc.format)
                == D3DFTL_PARTIAL_TYPE) {
                maybe_desc = D3D12_RENDER_TARGET_VIEW_DESC {
                    .Format = _rtv_format,
                    .ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D,
                    .Texture2D = D3D12_TEX2D_RTV {.MipSlice = 0, .PlaneSlice = 0},
                };
            }
            device.create_render_target_view(_resource, maybe_desc, _rtv_descriptor.cpu());
        }
        if constexpr (gpu_texture_flags_contains(FLAGS, Dsv)) {
            _dsv_descriptor = device.descriptors().dsv().alloc();
            std::optional<D3D12_DEPTH_STENCIL_VIEW_DESC> maybe_desc = std::nullopt;
            if (D3D12_PROPERTY_LAYOUT_FORMAT_TABLE::GetTypeLevel(desc.format)
                == D3DFTL_PARTIAL_TYPE) {
                maybe_desc = D3D12_DEPTH_STENCIL_VIEW_DESC {
                    .Format = _dsv_format,
                    .ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D,
                    .Flags = D3D12_DSV_FLAG_NONE,
                    .Texture2D = D3D12_TEX2D_DSV {.MipSlice = 0},
                };
            }
            device.create_depth_stencil_view(_resource, maybe_desc, _dsv_descriptor.cpu());
        }

        // Copy desc.
        _desc = desc;
    }
    auto create_and_transfer(
        GpuDevice& device,
        const GpuTextureDesc& desc,
        std::span<const GpuTextureTransferDesc> transfer_descs,
        D3D12_RESOURCE_STATES before_state,
        D3D12_RESOURCE_STATES after_state,
        std::string_view name
    ) -> void {
        create(device, desc, name);
        std::vector<D3D12_SUBRESOURCE_DATA> subresources(transfer_descs.size());
        for (uint32_t i = 0; i < transfer_descs.size(); i++) {
            subresources[i] = D3D12_SUBRESOURCE_DATA {
                .pData = transfer_descs[i].data,
                .RowPitch = (int64_t)transfer_descs[i].row_pitch,
                .SlicePitch = (int64_t)transfer_descs[i].slice_pitch,
            };
        }
        device.transfer().resource(_resource, subresources, before_state, after_state);
    }
    auto create_and_transfer(
        GpuDevice& device,
        const GpuTextureDesc& desc,
        const GpuTextureTransferDesc& transfer_desc,
        D3D12_RESOURCE_STATES before_state,
        D3D12_RESOURCE_STATES after_state,
        std::string_view name
    ) -> void {
        create_and_transfer(
            device,
            desc,
            std::span<const GpuTextureTransferDesc>(&transfer_desc, 1),
            before_state,
            after_state,
            name
        );
    }

    auto width() const -> uint32_t { return _desc.width; }
    auto height() const -> uint32_t { return _desc.height; }
    auto format() const -> DXGI_FORMAT { return _desc.format; }
    auto mip_count() const -> uint32_t { return _desc.mip_count; }
    auto bits_per_unit() const -> uint32_t {
        return D3D12_PROPERTY_LAYOUT_FORMAT_TABLE::GetBitsPerUnit(_desc.format);
    }
    auto bytes_per_unit() const -> uint32_t { return bits_per_unit() / 8; }
    auto resource() const -> const ComPtr<ID3D12Resource>& { return _resource; }
    auto row_pitch() const -> uint32_t { return bytes_per_unit() * _desc.width; }
    auto slice_pitch() const -> uint32_t { return row_pitch() * _desc.height; }
    auto srv_descriptor() const -> GpuDescriptor {
        static_assert(gpu_texture_flags_contains(FLAGS, Srv));
        return _srv_descriptor;
    }
    auto uav_descriptor() const -> GpuDescriptor {
        static_assert(gpu_texture_flags_contains(FLAGS, Uav));
        return _uav_descriptors[0];
    }
    auto rtv_descriptor() const -> GpuDescriptor {
        static_assert(gpu_texture_flags_contains(FLAGS, Rtv));
        return _rtv_descriptor;
    }
    auto dsv_descriptor() const -> GpuDescriptor {
        static_assert(gpu_texture_flags_contains(FLAGS, Dsv));
        return _dsv_descriptor;
    }

    auto transition(GpuCommandList& cmd, D3D12_RESOURCE_STATES after) -> void {
        if (_resource_state != after) {
            cmd.transition_barrier(_resource, _resource_state, after);
            _resource_state = after;
        }
    }

    auto uav_barrier(GpuCommandList& cmd) -> void {
        static_assert(gpu_texture_flags_contains(FLAGS, Uav));
        cmd.uav_barrier(_resource);
    }

private:
    GpuTextureDesc _desc;
    ComPtr<ID3D12Resource> _resource;
    D3D12_RESOURCE_STATES _resource_state = D3D12_RESOURCE_STATE_COMMON;
    DXGI_FORMAT _srv_format = DXGI_FORMAT_UNKNOWN;
    DXGI_FORMAT _uav_format = DXGI_FORMAT_UNKNOWN;
    DXGI_FORMAT _rtv_format = DXGI_FORMAT_UNKNOWN;
    DXGI_FORMAT _dsv_format = DXGI_FORMAT_UNKNOWN;
    GpuDescriptor _srv_descriptor = {};
    std::array<GpuDescriptor, MAX_MIP_COUNT> _uav_descriptors = {};
    GpuDescriptor _rtv_descriptor = {};
    GpuDescriptor _dsv_descriptor = {};
};

using GpuTextureSrv = GpuTexture<GpuTextureFlags::Srv>;
using GpuTextureUav = GpuTexture<GpuTextureFlags::Uav>;
using GpuTextureRtv = GpuTexture<GpuTextureFlags::Rtv>;
using GpuTextureDsv = GpuTexture<GpuTextureFlags::Dsv>;

using GpuTextureSrvUav = GpuTexture<GpuTextureFlags::SrvUav>;
using GpuTextureSrvRtv = GpuTexture<GpuTextureFlags::SrvRtv>;
using GpuTextureSrvDsv = GpuTexture<GpuTextureFlags::SrvDsv>;
using GpuTextureSrvCube = GpuTexture<GpuTextureFlags::SrvCube>;

using GpuTextureSrvUavRtv = GpuTexture<GpuTextureFlags::SrvUavRtv>;

inline auto mip_count_from_size(uint32_t width, uint32_t height) -> uint32_t {
    return 1 + (uint32_t)std::floor(std::log2(float(std::max(width, height))));
}

inline auto mip_count_from_size(Uint2 size) -> uint32_t {
    return mip_count_from_size(size.x, size.y);
}

} // namespace fb
