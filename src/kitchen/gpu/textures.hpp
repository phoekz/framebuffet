#pragma once

#include <common/common.hpp>
#include "device.hpp"
#include "formats.hpp"
#include <baked/baked_types.hpp>

namespace fb {

enum class GpuTextureFlags : uint {
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
    UavCube = Uav | Cube,
    SrvUavCube = Srv | Uav | Cube,
    SrvUavRtv = Srv | Uav | Rtv,
};

inline constexpr auto
gpu_texture_flags_contains(GpuTextureFlags flags, GpuTextureFlags flag) -> bool {
    return ((uint)flags & (uint)flag) == (uint)flag;
}

struct GpuTextureDesc {
    DXGI_FORMAT format = DXGI_FORMAT_UNKNOWN;
    uint width = 1;
    uint height = 1;
    uint depth = 1;
    uint mip_count = 1;
    uint sample_count = 1;
    Option<D3D12_CLEAR_VALUE> clear_value = std::nullopt;
    Option<DXGI_FORMAT> srv_format = std::nullopt;
    Option<DXGI_FORMAT> uav_format = std::nullopt;
    Option<DXGI_FORMAT> rtv_format = std::nullopt;
    Option<DXGI_FORMAT> dsv_format = std::nullopt;
};

struct GpuTextureTransferDesc {
    uint row_pitch = 0;
    uint slice_pitch = 0;
    const void* data = nullptr;
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
            FB_ASSERT(desc.depth > 0);
            FB_ASSERT((desc.depth % 6) == 0);
            FB_ASSERT(desc.sample_count == 1);
        }

        // Optionally override view formats.
        _srv_format = desc.srv_format.value_or(desc.format);
        _uav_format = desc.uav_format.value_or(desc.format);
        _rtv_format = desc.rtv_format.value_or(desc.format);
        _dsv_format = desc.dsv_format.value_or(desc.format);

        // Heap properties.
        const auto heap_type = D3D12_HEAP_TYPE_DEFAULT;

        // Texture flags.
        auto resource_flags = D3D12_RESOURCE_FLAG_NONE;
        auto initial_layout = D3D12_BARRIER_LAYOUT_UNDEFINED;
        if (gpu_texture_flags_contains(FLAGS, Uav)) {
            resource_flags |= D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;
        }
        if (gpu_texture_flags_contains(FLAGS, Rtv)) {
            resource_flags |= D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;
            initial_layout = D3D12_BARRIER_LAYOUT_RENDER_TARGET;
        }
        if (gpu_texture_flags_contains(FLAGS, Dsv)) {
            resource_flags |= D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;
            initial_layout = D3D12_BARRIER_LAYOUT_DEPTH_STENCIL_WRITE;
        }

        // Resource desc.
        D3D12_RESOURCE_DESC1 resource_desc {
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
            .SamplerFeedbackMipRegion = {},
        };

        // Clear value.
        auto clear_value = Option<D3D12_CLEAR_VALUE>(std::nullopt);
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
            heap_type,
            resource_desc,
            initial_layout,
            clear_value,
            name
        );
        _layout_before = initial_layout;

        // Views.
        if constexpr (gpu_texture_flags_contains(FLAGS, Srv)) {
            // Multisampling texture arrays.
            if (desc.sample_count > 1 && desc.depth > 1) {
                _srv_descriptor = device.descriptors().cbv_srv_uav().alloc();
                device.create_shader_resource_view(
                    _resource,
                    D3D12_SHADER_RESOURCE_VIEW_DESC {
                        .Format = _srv_format,
                        .ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2DMSARRAY,
                        .Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING,
                        .Texture2DMSArray =
                            D3D12_TEX2DMS_ARRAY_SRV {
                                .FirstArraySlice = 0,
                                .ArraySize = desc.depth,
                            },
                    },
                    _srv_descriptor.cpu()
                );
            }
            // Multisampling texture.
            else if (desc.sample_count > 1 && desc.depth == 1) {
                _srv_descriptor = device.descriptors().cbv_srv_uav().alloc();
                device.create_shader_resource_view(
                    _resource,
                    D3D12_SHADER_RESOURCE_VIEW_DESC {
                        .Format = _srv_format,
                        .ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2DMS,
                        .Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING,
                        .Texture2DMS =
                            D3D12_TEX2DMS_SRV {
                                .UnusedField_NothingToDefine = 0,
                            },
                    },
                    _srv_descriptor.cpu()
                );
            }
            // Cube texture arrays.
            else if (gpu_texture_flags_contains(FLAGS, Cube) && desc.depth > 6) {
                _srv_descriptor = device.descriptors().cbv_srv_uav().alloc();
                device.create_shader_resource_view(
                    _resource,
                    D3D12_SHADER_RESOURCE_VIEW_DESC {
                        .Format = _srv_format,
                        .ViewDimension = D3D12_SRV_DIMENSION_TEXTURECUBEARRAY,
                        .Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING,
                        .TextureCubeArray =
                            D3D12_TEXCUBE_ARRAY_SRV {
                                .MostDetailedMip = 0,
                                .MipLevels = desc.mip_count,
                                .First2DArrayFace = 0,
                                .NumCubes = desc.depth / 6,
                                .ResourceMinLODClamp = 0.0f,
                            },
                    },
                    _srv_descriptor.cpu()
                );

                _alt_srv_descriptor = device.descriptors().cbv_srv_uav().alloc();
                device.create_shader_resource_view(
                    _resource,
                    D3D12_SHADER_RESOURCE_VIEW_DESC {
                        .Format = _srv_format,
                        .ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2DARRAY,
                        .Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING,
                        .Texture2DArray =
                            D3D12_TEX2D_ARRAY_SRV {
                                .MostDetailedMip = 0,
                                .MipLevels = desc.mip_count,
                                .FirstArraySlice = 0,
                                .ArraySize = desc.depth,
                                .PlaneSlice = 0,
                                .ResourceMinLODClamp = 0.0f,
                            },
                    },
                    _alt_srv_descriptor.cpu()
                );
            }
            // Cube textures.
            else if (gpu_texture_flags_contains(FLAGS, Cube) && desc.depth == 6) {
                _srv_descriptor = device.descriptors().cbv_srv_uav().alloc();
                device.create_shader_resource_view(
                    _resource,
                    D3D12_SHADER_RESOURCE_VIEW_DESC {
                        .Format = _srv_format,
                        .ViewDimension = D3D12_SRV_DIMENSION_TEXTURECUBE,
                        .Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING,
                        .TextureCube =
                            D3D12_TEXCUBE_SRV {
                                .MostDetailedMip = 0,
                                .MipLevels = desc.mip_count,
                                .ResourceMinLODClamp = 0.0f,
                            },
                    },
                    _srv_descriptor.cpu()
                );

                _alt_srv_descriptor = device.descriptors().cbv_srv_uav().alloc();
                device.create_shader_resource_view(
                    _resource,
                    D3D12_SHADER_RESOURCE_VIEW_DESC {
                        .Format = _srv_format,
                        .ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2DARRAY,
                        .Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING,
                        .Texture2DArray =
                            D3D12_TEX2D_ARRAY_SRV {
                                .MostDetailedMip = 0,
                                .MipLevels = desc.mip_count,
                                .FirstArraySlice = 0,
                                .ArraySize = desc.depth,
                                .PlaneSlice = 0,
                                .ResourceMinLODClamp = 0.0f,
                            },
                    },
                    _alt_srv_descriptor.cpu()
                );
            }
            // Texture arrays.
            else if (desc.depth > 1) {
                _srv_descriptor = device.descriptors().cbv_srv_uav().alloc();
                device.create_shader_resource_view(
                    _resource,
                    D3D12_SHADER_RESOURCE_VIEW_DESC {
                        .Format = _srv_format,
                        .ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2DARRAY,
                        .Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING,
                        .Texture2DArray =
                            D3D12_TEX2D_ARRAY_SRV {
                                .MostDetailedMip = 0,
                                .MipLevels = desc.mip_count,
                                .FirstArraySlice = 0,
                                .ArraySize = desc.depth,
                                .PlaneSlice = 0,
                                .ResourceMinLODClamp = 0.0f,
                            },
                    },
                    _srv_descriptor.cpu()
                );
            }
            // Textures.
            else {
                _srv_descriptor = device.descriptors().cbv_srv_uav().alloc();
                device.create_shader_resource_view(
                    _resource,
                    D3D12_SHADER_RESOURCE_VIEW_DESC {
                        .Format = _srv_format,
                        .ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D,
                        .Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING,
                        .Texture2D =
                            D3D12_TEX2D_SRV {
                                .MostDetailedMip = 0,
                                .MipLevels = desc.mip_count,
                                .PlaneSlice = 0,
                                .ResourceMinLODClamp = 0.0f,
                            },
                    },
                    _srv_descriptor.cpu()
                );
            }
        }
        if constexpr (gpu_texture_flags_contains(FLAGS, Uav)) {
            if (desc.depth > 1) {
                for (uint mip = 0; mip < desc.mip_count; mip++) {
                    _uav_descriptors[mip] = device.descriptors().cbv_srv_uav().alloc();
                    device.create_unordered_access_view(
                        _resource,
                        std::nullopt,
                        D3D12_UNORDERED_ACCESS_VIEW_DESC {
                            .Format = _uav_format,
                            .ViewDimension = D3D12_UAV_DIMENSION_TEXTURE2DARRAY,
                            .Texture2DArray =
                                D3D12_TEX2D_ARRAY_UAV {
                                    .MipSlice = mip,
                                    .FirstArraySlice = 0,
                                    .ArraySize = desc.depth,
                                    .PlaneSlice = 0,
                                },
                        },
                        _uav_descriptors[mip].cpu()
                    );
                }
            } else {
                for (uint mip = 0; mip < desc.mip_count; mip++) {
                    _uav_descriptors[mip] = device.descriptors().cbv_srv_uav().alloc();
                    device.create_unordered_access_view(
                        _resource,
                        std::nullopt,
                        D3D12_UNORDERED_ACCESS_VIEW_DESC {
                            .Format = _uav_format,
                            .ViewDimension = D3D12_UAV_DIMENSION_TEXTURE2D,
                            .Texture2D =
                                D3D12_TEX2D_UAV {
                                    .MipSlice = mip,
                                    .PlaneSlice = 0,
                                },
                        },
                        _uav_descriptors[mip].cpu()
                    );
                }
            }
        }
        if constexpr (gpu_texture_flags_contains(FLAGS, Rtv)) {
            _rtv_descriptor = device.descriptors().rtv().alloc();
            const auto type_level = dxgi_format_type_level(desc.format);
            auto maybe_desc = Option<D3D12_RENDER_TARGET_VIEW_DESC>(std::nullopt);
            if (type_level == D3DFTL_FULL_TYPE || type_level == D3DFTL_PARTIAL_TYPE) {
                D3D12_RTV_DIMENSION view_dimension;
                if (desc.sample_count > 1) {
                    view_dimension = D3D12_RTV_DIMENSION_TEXTURE2DMS;
                } else {
                    view_dimension = D3D12_RTV_DIMENSION_TEXTURE2D;
                }
                maybe_desc = D3D12_RENDER_TARGET_VIEW_DESC {
                    .Format = _rtv_format,
                    .ViewDimension = view_dimension,
                    .Texture2D = D3D12_TEX2D_RTV {.MipSlice = 0, .PlaneSlice = 0},
                };
            }
            device.create_render_target_view(_resource, maybe_desc, _rtv_descriptor.cpu());
        }
        if constexpr (gpu_texture_flags_contains(FLAGS, Dsv)) {
            _dsv_descriptor = device.descriptors().dsv().alloc();
            const auto type_level = dxgi_format_type_level(desc.format);
            auto maybe_desc = Option<D3D12_DEPTH_STENCIL_VIEW_DESC>(std::nullopt);
            if (type_level == D3DFTL_FULL_TYPE || type_level == D3DFTL_PARTIAL_TYPE) {
                D3D12_DSV_DIMENSION view_dimension;
                if (desc.sample_count > 1) {
                    view_dimension = D3D12_DSV_DIMENSION_TEXTURE2DMS;
                } else {
                    view_dimension = D3D12_DSV_DIMENSION_TEXTURE2D;
                }
                maybe_desc = D3D12_DEPTH_STENCIL_VIEW_DESC {
                    .Format = _dsv_format,
                    .ViewDimension = view_dimension,
                    .Flags = D3D12_DSV_FLAG_NONE,
                    .Texture2D = D3D12_TEX2D_DSV {.MipSlice = 0},
                };
            }
            device.create_depth_stencil_view(_resource, maybe_desc, _dsv_descriptor.cpu());
        }

        // Compute tightly packed byte size.
        {
            uint64_t byte_count_64 = 0;
            for (uint mip = 0; mip < desc.mip_count; mip++) {
                for (uint slice = 0; slice < desc.depth; slice++) {
                    const auto mip_width = std::max(1u, desc.width >> mip);
                    const auto mip_height = std::max(1u, desc.height >> mip);
                    const auto unit_bytes = dxgi_format_unit_byte_count(desc.format);
                    byte_count_64 += mip_width * mip_height * unit_bytes;
                }
            }
            FB_ASSERT(byte_count_64 <= UINT32_MAX);
            _byte_count = (uint)byte_count_64;
        }

        // Copy desc.
        _desc = desc;
    }
    auto create_and_transfer(
        GpuDevice& device,
        const GpuTextureDesc& desc,
        Span<const GpuTextureTransferDesc> transfer_descs,
        D3D12_BARRIER_SYNC sync_after,
        D3D12_BARRIER_ACCESS access_after,
        D3D12_BARRIER_LAYOUT layout_after,
        std::string_view name
    ) -> void {
        create(device, desc, name);
        std::vector<D3D12_SUBRESOURCE_DATA> subresources(transfer_descs.size());
        for (uint i = 0; i < transfer_descs.size(); i++) {
            subresources[i] = D3D12_SUBRESOURCE_DATA {
                .pData = transfer_descs[i].data,
                .RowPitch = (int64_t)transfer_descs[i].row_pitch,
                .SlicePitch = (int64_t)transfer_descs[i].slice_pitch,
            };
        }
        device.transfer().resource(
            _resource,
            subresources,
            _sync_before,
            sync_after,
            _access_before,
            access_after,
            _layout_before,
            layout_after
        );
    }
    auto create_and_transfer(
        GpuDevice& device,
        const GpuTextureDesc& desc,
        const GpuTextureTransferDesc& transfer_desc,
        D3D12_BARRIER_SYNC sync_after,
        D3D12_BARRIER_ACCESS access_after,
        D3D12_BARRIER_LAYOUT layout_after,
        std::string_view name
    ) -> void {
        create_and_transfer(
            device,
            desc,
            Span<const GpuTextureTransferDesc>(&transfer_desc, 1),
            sync_after,
            access_after,
            layout_after,
            name
        );
    }
    auto create_and_transfer_baked(
        GpuDevice& device,
        const baked::Texture& baked,
        D3D12_BARRIER_SYNC sync_after,
        D3D12_BARRIER_ACCESS access_after,
        D3D12_BARRIER_LAYOUT layout_after,
        std::string_view name
    ) {
        GpuTextureDesc desc = {
            .format = baked.format,
            .width = baked.width,
            .height = baked.height,
            .mip_count = baked.mip_count,
            .sample_count = 1,
        };

        std::array<GpuTextureTransferDesc, baked::MAX_MIP_COUNT> transfer_descs = {};
        for (uint mip = 0; mip < baked.mip_count; mip++) {
            transfer_descs[mip] = GpuTextureTransferDesc {
                .row_pitch = baked.datas[mip].row_pitch,
                .slice_pitch = baked.datas[mip].slice_pitch,
                .data = baked.datas[mip].data.data(),
            };
        }

        create_and_transfer(
            device,
            desc,
            Span(transfer_descs.data(), baked.mip_count),
            sync_after,
            access_after,
            layout_after,
            name
        );
    }
    auto create_and_transfer_baked(
        GpuDevice& device,
        const baked::CubeTexture& baked,
        D3D12_BARRIER_SYNC sync_after,
        D3D12_BARRIER_ACCESS access_after,
        D3D12_BARRIER_LAYOUT layout_after,
        std::string_view name
    ) {
        GpuTextureDesc desc = {
            .format = baked.format,
            .width = baked.width,
            .height = baked.height,
            .depth = 6,
            .mip_count = baked.mip_count,
            .sample_count = 1,
        };

        std::array<GpuTextureTransferDesc, 6 * baked::MAX_MIP_COUNT> transfer_descs = {};
        uint desc_count = 0;
        for (uint slice = 0; slice < 6; slice++) {
            for (uint mip = 0; mip < baked.mip_count; mip++) {
                transfer_descs[desc_count] = GpuTextureTransferDesc {
                    .row_pitch = baked.datas[slice][mip].row_pitch,
                    .slice_pitch = baked.datas[slice][mip].slice_pitch,
                    .data = baked.datas[slice][mip].data.data(),
                };
                desc_count++;
            }
        }

        create_and_transfer(
            device,
            desc,
            Span(transfer_descs.data(), desc_count),
            sync_after,
            access_after,
            layout_after,
            name
        );
    }

    auto width() const -> uint { return _desc.width; }
    auto height() const -> uint { return _desc.height; }
    auto depth() const -> uint { return _desc.depth; }
    auto size() const -> uint2 { return uint2 {_desc.width, _desc.height}; }
    auto format() const -> DXGI_FORMAT { return _desc.format; }
    auto mip_count() const -> uint { return _desc.mip_count; }
    auto unit_byte_count() const -> uint { return dxgi_format_unit_byte_count(_desc.format); }
    auto resource() const -> const ComPtr<ID3D12Resource2>& { return _resource; }
    auto row_pitch() const -> uint { return unit_byte_count() * _desc.width; }
    auto slice_pitch() const -> uint { return row_pitch() * _desc.height; }
    auto byte_count() const -> uint { return _byte_count; }
    auto srv_descriptor() const -> GpuDescriptor {
        static_assert(gpu_texture_flags_contains(FLAGS, Srv));
        return _srv_descriptor;
    }
    auto alt_srv_descriptor() const -> GpuDescriptor {
        static_assert(gpu_texture_flags_contains(FLAGS, SrvCube));
        return _alt_srv_descriptor;
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

    auto transition(
        GpuCommandList& cmd,
        D3D12_BARRIER_SYNC sync_after,
        D3D12_BARRIER_ACCESS access_after,
        D3D12_BARRIER_LAYOUT layout_after
    ) -> void {
        explicit_transition(
            cmd,
            _sync_before,
            sync_after,
            _access_before,
            access_after,
            _layout_before,
            layout_after
        );
    }

    auto explicit_transition(
        GpuCommandList& cmd,
        D3D12_BARRIER_SYNC sync_before,
        D3D12_BARRIER_SYNC sync_after,
        D3D12_BARRIER_ACCESS access_before,
        D3D12_BARRIER_ACCESS access_after,
        D3D12_BARRIER_LAYOUT layout_before,
        D3D12_BARRIER_LAYOUT layout_after
    ) -> void {
        cmd.texture_barrier(
            sync_before,
            sync_after,
            access_before,
            access_after,
            layout_before,
            layout_after,
            _resource,
            D3D12_BARRIER_SUBRESOURCE_RANGE {
                .IndexOrFirstMipLevel = 0xffffffffu,
                .NumMipLevels = 0,
                .FirstArraySlice = 0,
                .NumArraySlices = 0,
                .FirstPlane = 0,
                .NumPlanes = 0,
            }
        );

        _sync_before = sync_after;
        _access_before = access_after;
        _layout_before = layout_after;
    }

private:
    GpuTextureDesc _desc;
    ComPtr<ID3D12Resource2> _resource;
    D3D12_BARRIER_SYNC _sync_before = D3D12_BARRIER_SYNC_NONE;
    D3D12_BARRIER_ACCESS _access_before = D3D12_BARRIER_ACCESS_NO_ACCESS;
    D3D12_BARRIER_LAYOUT _layout_before = D3D12_BARRIER_LAYOUT_UNDEFINED;
    DXGI_FORMAT _srv_format = DXGI_FORMAT_UNKNOWN;
    DXGI_FORMAT _uav_format = DXGI_FORMAT_UNKNOWN;
    DXGI_FORMAT _rtv_format = DXGI_FORMAT_UNKNOWN;
    DXGI_FORMAT _dsv_format = DXGI_FORMAT_UNKNOWN;
    GpuDescriptor _srv_descriptor = {};
    GpuDescriptor _alt_srv_descriptor = {};
    std::array<GpuDescriptor, MAX_MIP_COUNT> _uav_descriptors = {};
    GpuDescriptor _rtv_descriptor = {};
    GpuDescriptor _dsv_descriptor = {};
    uint _byte_count = 0;
};

using GpuTextureSrv = GpuTexture<GpuTextureFlags::Srv>;
using GpuTextureUav = GpuTexture<GpuTextureFlags::Uav>;
using GpuTextureRtv = GpuTexture<GpuTextureFlags::Rtv>;
using GpuTextureDsv = GpuTexture<GpuTextureFlags::Dsv>;

using GpuTextureSrvUav = GpuTexture<GpuTextureFlags::SrvUav>;
using GpuTextureSrvRtv = GpuTexture<GpuTextureFlags::SrvRtv>;
using GpuTextureSrvDsv = GpuTexture<GpuTextureFlags::SrvDsv>;
using GpuTextureSrvCube = GpuTexture<GpuTextureFlags::SrvCube>;
using GpuTextureUavCube = GpuTexture<GpuTextureFlags::UavCube>;

using GpuTextureSrvUavCube = GpuTexture<GpuTextureFlags::SrvUavCube>;
using GpuTextureSrvUavRtv = GpuTexture<GpuTextureFlags::SrvUavRtv>;

} // namespace fb
