#pragma once

#include "../gpu/gpu.hpp"

namespace fb::graphics::render_targets {

struct RenderTargetsDesc {
    uint2 size = {};
    DXGI_FORMAT color_format = DXGI_FORMAT_R8G8B8A8_UNORM;
    float4 color_clear_value = {};
    Option<DXGI_FORMAT> depth_format = std::nullopt;
    Option<float> depth_clear_value = std::nullopt;
    uint sample_count = 1;
};

class RenderTargets {
    FB_NO_COPY_MOVE(RenderTargets);

public:
    RenderTargets() = default;

    auto create(GpuDevice& device, const RenderTargetsDesc& desc) -> void;
    auto transition_to_render_target(GpuCommandList& cmd) -> void;
    auto clear(GpuCommandList& cmd) -> void;
    auto transition_to_resolve(GpuCommandList& cmd) -> void;
    auto resolve(GpuCommandList& cmd) -> void;
    auto transition_to_shader_resource(GpuCommandList& cmd) -> void;

    auto set(GpuGraphicsCommandList& cmd) -> void;

    auto color() const -> const GpuTextureSrvUavRtv& { return _color; }
    auto depth() const -> const Option<ConstRef<GpuTextureDsv>> { return _depth; }
    auto color_format() const -> DXGI_FORMAT { return _color.format(); }
    auto depth_format() const -> Option<DXGI_FORMAT> {
        Option<DXGI_FORMAT> format;
        if (_has_depth) {
            format = _depth.format();
        }
        return format;
    }
    auto sample_desc() const -> DXGI_SAMPLE_DESC { return {.Count = _sample_count}; }

private:
    uint2 _size = {};
    float4 _color_clear_value = {};
    float _depth_clear_value = 1.0f;
    GpuTextureRtv _multisampled_color;
    GpuTextureSrvUavRtv _color;
    GpuTextureDsv _depth;
    bool _has_depth = false;
    uint _sample_count = 1;
};

} // namespace fb::graphics::render_targets
