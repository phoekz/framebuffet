#pragma once

#include <pch.hpp>
#include "device.hpp"
#include "utils.hpp"

namespace fb {

enum class GpuTextureFlags : uint32_t {
    None = 0x0,
    Srv = 0x1,
    Uav = 0x2,
    Rtv = 0x4,
    Dsv = 0x8,
    Cube = 0x10,
    SrvRtv = Srv | Rtv,
    SrvDsv = Srv | Dsv,
    SrvCube = Srv | Cube,
};

constexpr auto gpu_texture_flags_is_set(GpuTextureFlags flags, GpuTextureFlags flag) -> bool {
    return ((uint32_t)flags & (uint32_t)flag) > 0;
}

struct GpuTexture2dDesc {
    DXGI_FORMAT format = DXGI_FORMAT_UNKNOWN;
    uint32_t width = 1;
    uint32_t height = 1;
    uint32_t depth = 1;
    uint32_t mip_levels = 1;
    uint32_t sample_count = 1;
    std::optional<D3D12_CLEAR_VALUE> clear_value = std::nullopt;
    std::optional<DXGI_FORMAT> srv_format = std::nullopt;
    std::optional<DXGI_FORMAT> dsv_format = std::nullopt;
};

template<GpuTextureFlags FLAGS>
class GpuTexture2d {
  public:
    auto create(const GpuDevice& device, const GpuTexture2dDesc& desc, std::string_view name)
        -> void {
        // Validate.
        FB_ASSERT(desc.format != DXGI_FORMAT_UNKNOWN);
        FB_ASSERT(desc.width > 0);
        FB_ASSERT(desc.height > 0);
        FB_ASSERT(desc.depth > 0);
        FB_ASSERT(desc.mip_levels > 0);
        FB_ASSERT(desc.sample_count > 0);
        if (gpu_texture_flags_is_set(FLAGS, GpuTextureFlags::Cube)) {
            FB_ASSERT(desc.depth == 6);
        }

        // Copy desc.
        _desc = desc;

        // Optionally override view formats.
        _srv_format = desc.srv_format.value_or(desc.format);
        _dsv_format = desc.dsv_format.value_or(desc.format);

        // Heap properties.
        const auto heap_props = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);

        // Texture flags.
        D3D12_RESOURCE_FLAGS resource_flags = D3D12_RESOURCE_FLAG_NONE;
        D3D12_RESOURCE_STATES init_state = D3D12_RESOURCE_STATE_COMMON;
        if (gpu_texture_flags_is_set(FLAGS, GpuTextureFlags::Uav)) {
            resource_flags |= D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;
        }
        if (gpu_texture_flags_is_set(FLAGS, GpuTextureFlags::Rtv)) {
            resource_flags |= D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;
            init_state = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
        }
        if (gpu_texture_flags_is_set(FLAGS, GpuTextureFlags::Dsv)) {
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
            .MipLevels = (uint16_t)desc.mip_levels,
            .Format = desc.format,
            .SampleDesc = DXGI_SAMPLE_DESC {.Count = desc.sample_count, .Quality = 0},
            .Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN,
            .Flags = resource_flags,
        };

        // Clear value.
        std::optional<D3D12_CLEAR_VALUE> clear_value = std::nullopt;
        if (gpu_texture_flags_is_set(FLAGS, GpuTextureFlags::Rtv)) {
            FB_ASSERT(desc.clear_value.has_value());
            clear_value = desc.clear_value;
        } else if (gpu_texture_flags_is_set(FLAGS, GpuTextureFlags::Dsv)) {
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
            name);
    }

    auto width() const -> uint32_t { return _desc.width; }
    auto height() const -> uint32_t { return _desc.height; }
    auto format() const -> DXGI_FORMAT { return _desc.format; }
    auto bits_per_unit() const -> uint32_t {
        return D3D12_PROPERTY_LAYOUT_FORMAT_TABLE::GetBitsPerUnit(_desc.format);
    }
    auto bytes_per_unit() const -> uint32_t { return bits_per_unit() / 8; }
    auto resource() const -> const ComPtr<ID3D12Resource>& { return _resource; }
    auto row_pitch() const -> uint32_t { return bytes_per_unit() * _desc.width; }
    auto slice_pitch() const -> uint32_t { return row_pitch() * _desc.height; }

    auto srv_desc() const -> D3D12_SHADER_RESOURCE_VIEW_DESC {
        static_assert(
            gpu_texture_flags_is_set(FLAGS, GpuTextureFlags::Srv),
            "Texture does not support SRV");
        D3D12_SRV_DIMENSION view_dimension;
        if (gpu_texture_flags_is_set(FLAGS, GpuTextureFlags::Cube)) {
            view_dimension = D3D12_SRV_DIMENSION_TEXTURECUBE;
        } else {
            view_dimension = D3D12_SRV_DIMENSION_TEXTURE2D;
        }

        return D3D12_SHADER_RESOURCE_VIEW_DESC {
            .Format = _srv_format,
            .ViewDimension = view_dimension,
            .Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING,
            .Texture2D =
                D3D12_TEX2D_SRV {
                    .MostDetailedMip = 0,
                    .MipLevels = _desc.mip_levels,
                    .PlaneSlice = 0,
                    .ResourceMinLODClamp = 0.0f,
                },
        };
    }

    auto dsv_desc() -> const D3D12_DEPTH_STENCIL_VIEW_DESC {
        static_assert(
            gpu_texture_flags_is_set(FLAGS, GpuTextureFlags::Dsv),
            "Texture does not support DSV");
        return D3D12_DEPTH_STENCIL_VIEW_DESC {
            .Format = _dsv_format,
            .ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D,
            .Flags = D3D12_DSV_FLAG_NONE,
            .Texture2D = D3D12_TEX2D_DSV {.MipSlice = 0},
        };
    }

  private:
    GpuTexture2dDesc _desc;
    ComPtr<ID3D12Resource> _resource;
    DXGI_FORMAT _srv_format = DXGI_FORMAT_UNKNOWN;
    DXGI_FORMAT _dsv_format = DXGI_FORMAT_UNKNOWN;
};

using GpuTexture2dSrv = GpuTexture2d<GpuTextureFlags::Srv>;
using GpuTexture2dUav = GpuTexture2d<GpuTextureFlags::Uav>;
using GpuTexture2dRtv = GpuTexture2d<GpuTextureFlags::Rtv>;
using GpuTexture2dDsv = GpuTexture2d<GpuTextureFlags::Dsv>;

using GpuTexture2dSrvRtv = GpuTexture2d<GpuTextureFlags::SrvRtv>;
using GpuTexture2dSrvDsv = GpuTexture2d<GpuTextureFlags::SrvDsv>;
using GpuTexture2dSrvCube = GpuTexture2d<GpuTextureFlags::SrvCube>;

}  // namespace fb
