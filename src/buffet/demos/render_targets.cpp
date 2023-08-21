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

RenderTargets::RenderTargets(
    GpuDevice& device,
    const RenderTargetsDesc& desc,
    std::string_view name
)
    : _size(desc.size)
    , _clear_color(desc.clear_color)
    , _sample_count(desc.sample_count) {
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

auto RenderTargets::begin(GpuDevice&, const GpuCommandList& cmd) -> void {
    cmd.set_viewport(0, 0, _size.x, _size.y);
    cmd.set_scissor(0, 0, _size.x, _size.y);

    if (_sample_count > 1) {
        _multisampled_color.transition(cmd, D3D12_RESOURCE_STATE_RENDER_TARGET);
        cmd.set_rtv_dsv(_multisampled_color.rtv_descriptor(), _depth.dsv_descriptor());
        cmd.clear_rtv(_multisampled_color.rtv_descriptor(), _clear_color);
        cmd.clear_dsv(_depth.dsv_descriptor(), DEPTH_VALUE);
    } else {
        _color.transition(cmd, D3D12_RESOURCE_STATE_RENDER_TARGET);
        cmd.set_rtv_dsv(_color.rtv_descriptor(), _depth.dsv_descriptor());
        cmd.clear_rtv(_color.rtv_descriptor(), _clear_color);
        cmd.clear_dsv(_depth.dsv_descriptor(), DEPTH_VALUE);
    }
}

auto RenderTargets::end(GpuDevice&, const GpuCommandList& cmd) -> void {
    if (_sample_count > 1) {
        _multisampled_color.transition(cmd, D3D12_RESOURCE_STATE_RESOLVE_SOURCE);
        _color.transition(cmd, D3D12_RESOURCE_STATE_RESOLVE_DEST);
        cmd.resolve_resource(
            _color.resource(),
            _multisampled_color.resource(),
            _multisampled_color.format()
        );
        _color.transition(cmd, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
    } else {
        _color.transition(cmd, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
    }
}

} // namespace fb::demos
