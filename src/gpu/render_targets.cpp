#include "render_targets.hpp"

namespace fb {

static auto make_color_clear_value(DXGI_FORMAT format, Vector4 color) -> D3D12_CLEAR_VALUE {
    return D3D12_CLEAR_VALUE {.Format = format, .Color = {color.x, color.y, color.z, color.w}};
}

static auto make_depth_stencil_clear_value(DXGI_FORMAT format, float depth, uint8_t stencil)
    -> D3D12_CLEAR_VALUE {
    return D3D12_CLEAR_VALUE {
        .Format = format,
        .DepthStencil = {.Depth = depth, .Stencil = stencil}};
}

GpuRenderTargets::GpuRenderTargets(
    GpuDevice& device,
    Uint2 size,
    Vector4 clear_color,
    std::string_view name) :
    _size(size),
    _clear_color(clear_color) {
    _color.create(
        device,
        GpuTexture2dDesc {
            .format = COLOR_FORMAT,
            .width = size.x,
            .height = size.y,
            .sample_count = SAMPLE_COUNT,
            .clear_value = make_color_clear_value(COLOR_FORMAT, _clear_color),
        },
        dx_name(name, "Color Target"));
    _depth.create(
        device,
        GpuTexture2dDesc {
            .format = DEPTH_FORMAT,
            .width = size.x,
            .height = size.y,
            .sample_count = SAMPLE_COUNT,
            .clear_value = make_depth_stencil_clear_value(DEPTH_FORMAT, 1.0f, 0),
        },
        dx_name(name, "Depth Target"));
}

auto GpuRenderTargets::begin(GpuDevice&, const GpuCommandList& cmd) -> void {
    _color.transition(cmd, D3D12_RESOURCE_STATE_RENDER_TARGET);
    cmd.set_viewport(0, 0, _size.x, _size.y);
    cmd.set_scissor(0, 0, _size.x, _size.y);
    cmd.set_rtv_dsv(_color.rtv_descriptor(), _depth.dsv_descriptor());
    cmd.clear_rtv(_color.rtv_descriptor(), _clear_color);
    cmd.clear_dsv(_depth.dsv_descriptor(), DEPTH_VALUE);
}

auto GpuRenderTargets::end(GpuDevice&, const GpuCommandList& cmd) -> void {
    _color.transition(cmd, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
}

}  // namespace fb
