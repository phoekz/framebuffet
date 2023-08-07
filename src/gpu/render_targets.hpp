#pragma once

#include <pch.hpp>
#include "device.hpp"
#include "textures.hpp"
#include "maths.hpp"
#include "utils.hpp"

namespace fb {

class GpuRenderTargets {
  public:
    static constexpr DXGI_FORMAT COLOR_FORMAT = DXGI_FORMAT_R8G8B8A8_UNORM;
    static constexpr DXGI_FORMAT DEPTH_FORMAT = DXGI_FORMAT_D32_FLOAT;
    static constexpr uint32_t SAMPLE_COUNT = 1;

    GpuRenderTargets(
        const GpuDevice& device,
        GpuDescriptors& descriptors,
        Uint2 size,
        Vector4 clear_color,
        std::string_view name);

    auto begin(GpuDevice& device) -> void;
    auto end(GpuDevice& device) -> void;
    auto color() const -> const GpuTexture2dSrvRtv& { return _color; }
    auto depth() const -> const GpuTexture2dSrvDsv& { return _depth; }

  private:
    Uint2 _size = {};
    Vector4 _clear_color;

    GpuTexture2dSrvRtv _color;
    GpuDescriptorHandle _color_descriptor;

    GpuTexture2dSrvDsv _depth;
    GpuDescriptorHandle _depth_descriptor;
};

}  // namespace fb
