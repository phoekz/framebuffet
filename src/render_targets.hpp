#pragma once

#include "dx12.hpp"
#include "utils.hpp"
#include "maths.hpp"
#include "descriptors.hpp"
#include <pch.hpp>

namespace fb {

class GpuRenderTargets {
  public:
    static constexpr DXGI_FORMAT COLOR_FORMAT = DXGI_FORMAT_R8G8B8A8_UNORM;
    static constexpr DXGI_FORMAT DEPTH_FORMAT = DXGI_FORMAT_D32_FLOAT;
    static constexpr uint32_t SAMPLE_COUNT = 1;

    GpuRenderTargets(
        Dx& dx,
        GpuDescriptors& descriptors,
        uint32_t width,
        uint32_t height,
        Vector4 clear_color,
        std::string_view name);

    auto begin(Dx& dx) -> void;
    auto end(Dx& dx) -> void;
    auto color() const -> const ComPtr<ID3D12Resource>& { return _color; }
    auto depth() const -> const ComPtr<ID3D12Resource>& { return _depth; }

  private:
    uint32_t _width = 0;
    uint32_t _height = 0;
    Vector4 _clear_color;

    ComPtr<ID3D12Resource> _color;
    GpuDescriptorHandle _color_descriptor;

    ComPtr<ID3D12Resource> _depth;
    GpuDescriptorHandle _depth_descriptor;
};

}  // namespace fb
