#pragma once

#include <pch.hpp>
#include "device.hpp"
#include "descriptors.hpp"
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
    auto color() const -> const ComPtr<ID3D12Resource>& { return _color; }
    auto depth() const -> const ComPtr<ID3D12Resource>& { return _depth; }

  private:
    Uint2 _size = {};
    Vector4 _clear_color;

    ComPtr<ID3D12Resource> _color;
    GpuDescriptorHandle _color_descriptor;

    ComPtr<ID3D12Resource> _depth;
    GpuDescriptorHandle _depth_descriptor;
};

}  // namespace fb
