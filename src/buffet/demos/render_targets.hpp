#pragma once

#include "demos.hpp"

namespace fb::demos {

struct RenderTargetsDesc {
    Uint2 size;
    DXGI_FORMAT color_format;
    Float4 clear_color;
    uint32_t sample_count;
};

class RenderTargets {
    FB_NO_COPY_MOVE(RenderTargets);

public:
    RenderTargets(GpuDevice& device, const RenderTargetsDesc& desc, std::string_view name);

    auto begin(GpuDevice& device, const GpuCommandList& cmd) -> void;
    auto end(GpuDevice& device, const GpuCommandList& cmd) -> void;
    auto color() const -> const GpuTextureSrvUavRtv& { return _color; }
    auto depth() const -> const GpuTextureDsv& { return _depth; }
    auto color_format() const -> DXGI_FORMAT { return _color.format(); }
    auto depth_format() const -> DXGI_FORMAT { return _depth.format(); }
    auto sample_desc() const -> DXGI_SAMPLE_DESC { return {.Count = _sample_count}; }

private:
    static constexpr DXGI_FORMAT DEPTH_FORMAT = DXGI_FORMAT_D32_FLOAT;
    static constexpr float DEPTH_VALUE = 1.0f;

    Uint2 _size = {};
    Float4 _clear_color;
    GpuTextureRtv _multisampled_color;
    GpuTextureSrvUavRtv _color;
    GpuTextureDsv _depth;
    uint32_t _sample_count = 1;
};

} // namespace fb::demos
