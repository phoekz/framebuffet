#include "render_targets.hpp"

namespace fb::graphics::render_targets {

static auto make_color_clear_value(DXGI_FORMAT format, float4 color) -> D3D12_CLEAR_VALUE {
    return D3D12_CLEAR_VALUE {.Format = format, .Color = {color.x, color.y, color.z, color.w}};
}

static auto make_depth_stencil_clear_value(DXGI_FORMAT format, float depth, uint8_t stencil)
    -> D3D12_CLEAR_VALUE {
    return D3D12_CLEAR_VALUE {
        .Format = format,
        .DepthStencil = {.Depth = depth, .Stencil = stencil},
    };
}

auto RenderTargets::create(GpuDevice& device, const RenderTargetsDesc& desc) -> void {
    ZoneScoped;
    DebugScope debug("Render Targets");

    _size = desc.size;
    _color_clear_value = desc.color_clear_value;
    _depth_clear_value = desc.depth_clear_value.value_or(1.0f);
    _sample_count = desc.sample_count;

    if (desc.sample_count > 1) {
        _multisampled_color.create(
            device,
            GpuTextureDesc {
                .format = desc.color_format,
                .width = desc.size.x,
                .height = desc.size.y,
                .sample_count = desc.sample_count,
                .clear_value = make_color_clear_value(desc.color_format, _color_clear_value),
            },
            debug.with_name("Multisampled Color")
        );
    }

    _color.create(
        device,
        GpuTextureDesc {
            .format = desc.color_format,
            .width = desc.size.x,
            .height = desc.size.y,
            .mip_count = mip_count_from_size(desc.size),
            .sample_count = 1,
            .clear_value = make_color_clear_value(desc.color_format, _color_clear_value),
        },
        debug.with_name("Color")
    );

    if (desc.depth_format.has_value()) {
        _has_depth = true;
        _depth.create(
            device,
            GpuTextureDesc {
                .format = desc.depth_format.value(),
                .width = desc.size.x,
                .height = desc.size.y,
                .sample_count = desc.sample_count,
                .clear_value = make_depth_stencil_clear_value(
                    desc.depth_format.value(),
                    _depth_clear_value,
                    0
                ),
            },
            debug.with_name("Depth")
        );
    }
}

auto RenderTargets::transition_to_render_target(GpuCommandList& cmd) -> void {
    if (_sample_count > 1) {
        _multisampled_color.transition(
            cmd,
            D3D12_BARRIER_SYNC_RENDER_TARGET,
            D3D12_BARRIER_ACCESS_RENDER_TARGET,
            D3D12_BARRIER_LAYOUT_RENDER_TARGET
        );
    } else {
        _color.transition(
            cmd,
            D3D12_BARRIER_SYNC_RENDER_TARGET,
            D3D12_BARRIER_ACCESS_RENDER_TARGET,
            D3D12_BARRIER_LAYOUT_RENDER_TARGET
        );
    }
}

auto RenderTargets::clear(GpuCommandList& cmd) -> void {
    if (_sample_count > 1) {
        cmd.clear_rtv(_multisampled_color.rtv_descriptor(), _color_clear_value);
    } else {
        cmd.clear_rtv(_color.rtv_descriptor(), _color_clear_value);
    }

    if (_has_depth) {
        cmd.clear_dsv(_depth.dsv_descriptor(), _depth_clear_value);
    }
}

auto RenderTargets::transition_to_resolve(GpuCommandList& cmd) -> void {
    if (_sample_count > 1) {
        _multisampled_color.transition(
            cmd,
            D3D12_BARRIER_SYNC_RESOLVE,
            D3D12_BARRIER_ACCESS_RESOLVE_SOURCE,
            D3D12_BARRIER_LAYOUT_RESOLVE_SOURCE
        );
        _color.transition(
            cmd,
            D3D12_BARRIER_SYNC_RESOLVE,
            D3D12_BARRIER_ACCESS_RESOLVE_DEST,
            D3D12_BARRIER_LAYOUT_RESOLVE_DEST
        );
    }
}

auto RenderTargets::resolve(GpuCommandList& cmd) -> void {
    if (_sample_count > 1) {
        cmd.resolve_resource(
            _color.resource(),
            _multisampled_color.resource(),
            _multisampled_color.format()
        );
    }
}

auto RenderTargets::transition_to_shader_resource(GpuCommandList& cmd) -> void {
    _color.transition(
        cmd,
        D3D12_BARRIER_SYNC_PIXEL_SHADING,
        D3D12_BARRIER_ACCESS_SHADER_RESOURCE,
        D3D12_BARRIER_LAYOUT_DIRECT_QUEUE_SHADER_RESOURCE
    );
}

auto RenderTargets::set(GpuGraphicsCommandList& cmd) -> void {
    cmd.set_viewport(0, 0, _size.x, _size.y);
    cmd.set_scissor(0, 0, _size.x, _size.y);

    GpuDescriptor rtv;
    if (_sample_count > 1) {
        rtv = _multisampled_color.rtv_descriptor();
    } else {
        rtv = _color.rtv_descriptor();
    }

    Option<GpuDescriptor> dsv;
    if (_has_depth) {
        dsv = _depth.dsv_descriptor();
    }

    cmd.set_rtv_dsv(rtv, dsv);
}

} // namespace fb::graphics::render_targets
