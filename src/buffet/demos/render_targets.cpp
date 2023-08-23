#include "render_targets.hpp"

namespace fb::demos {

static auto make_color_clear_value(DXGI_FORMAT format, Float4 color) -> D3D12_CLEAR_VALUE {
    return D3D12_CLEAR_VALUE {.Format = format, .Color = {color.x, color.y, color.z, color.w}};
}

static auto make_depth_stencil_clear_value(DXGI_FORMAT format, float depth, uint8_t stencil)
    -> D3D12_CLEAR_VALUE {
    return D3D12_CLEAR_VALUE {
        .Format = format,
        .DepthStencil = {.Depth = depth, .Stencil = stencil},
    };
}

auto RenderTargets::create(GpuDevice& device, const RenderTargetsDesc& desc, std::string_view name)
    -> void {
    _size = desc.size;
    _clear_color = desc.clear_color;
    _sample_count = desc.sample_count;

    if (desc.sample_count > 1) {
        _multisampled_color.create(
            device,
            GpuTextureDesc {
                .format = desc.color_format,
                .width = desc.size.x,
                .height = desc.size.y,
                .sample_count = desc.sample_count,
                .clear_value = make_color_clear_value(desc.color_format, _clear_color),
            },
            dx_name(name, "Multisampled Color Target")
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
            .clear_value = make_color_clear_value(desc.color_format, _clear_color),
        },
        dx_name(name, "Color Target")
    );

    _depth.create(
        device,
        GpuTextureDesc {
            .format = DEPTH_FORMAT,
            .width = desc.size.x,
            .height = desc.size.y,
            .sample_count = desc.sample_count,
            .clear_value = make_depth_stencil_clear_value(DEPTH_FORMAT, DEPTH_VALUE, 0),
        },
        dx_name(name, "Depth Target")
    );
}

auto RenderTargets::transition_to_render_target(GpuCommandList& cmd) -> void {
    if (_sample_count > 1) {
        _multisampled_color.transition(cmd, D3D12_RESOURCE_STATE_RENDER_TARGET);
    } else {
        _color.transition(
            cmd,
            D3D12_RESOURCE_STATE_RENDER_TARGET
        ); // this is transitioning too much
    }
}

auto RenderTargets::clear_all(GpuCommandList& cmd) -> void {
    if (_sample_count > 1) {
        cmd.clear_rtv(_multisampled_color.rtv_descriptor(), _clear_color);
    } else {
        cmd.clear_rtv(_color.rtv_descriptor(), _clear_color);
    }
    cmd.clear_dsv(_depth.dsv_descriptor(), DEPTH_VALUE);
}

auto RenderTargets::transition_to_resolve(GpuCommandList& cmd) -> void {
    if (_sample_count > 1) {
        _multisampled_color.transition(cmd, D3D12_RESOURCE_STATE_RESOLVE_SOURCE);
        _color.transition(cmd, D3D12_RESOURCE_STATE_RESOLVE_DEST);
    }
}

auto RenderTargets::resolve_all(GpuCommandList& cmd) -> void {
    if (_sample_count > 1) {
        cmd.resolve_resource(
            _color.resource(),
            _multisampled_color.resource(),
            _multisampled_color.format()
        );
    }
}

auto RenderTargets::transition_to_pixel_shader_resource(GpuCommandList& cmd) -> void {
    _color.transition(cmd, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
}

auto RenderTargets::set(GpuCommandList& cmd) -> void {
    cmd.set_viewport(0, 0, _size.x, _size.y);
    cmd.set_scissor(0, 0, _size.x, _size.y);

    if (_sample_count > 1) {
        cmd.set_rtv_dsv(_multisampled_color.rtv_descriptor(), _depth.dsv_descriptor());
    } else {
        cmd.set_rtv_dsv(_color.rtv_descriptor(), _depth.dsv_descriptor());
    }
}

} // namespace fb::demos
