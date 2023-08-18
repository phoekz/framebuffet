#pragma once

#include "demos.hpp"

namespace fb::demos {

class RenderTargets {
  public:
    static constexpr DXGI_FORMAT COLOR_FORMAT = DXGI_FORMAT_R8G8B8A8_UNORM;
    static constexpr DXGI_FORMAT DEPTH_FORMAT = DXGI_FORMAT_D32_FLOAT;
    static constexpr uint32_t SAMPLE_COUNT = 1;
    static constexpr float DEPTH_VALUE = 1.0f;

    RenderTargets(GpuDevice& device, Uint2 size, Float4 clear_color, std::string_view name);

    auto begin(GpuDevice& device, const GpuCommandList& cmd) -> void;
    auto end(GpuDevice& device, const GpuCommandList& cmd) -> void;
    auto color() const -> const GpuTextureSrvRtv& { return _color; }
    auto depth() const -> const GpuTextureDsv& { return _depth; }

  private:
    Uint2 _size = {};
    Float4 _clear_color;
    GpuTextureSrvRtv _color;
    GpuTextureDsv _depth;
};

} // namespace fb::demos
