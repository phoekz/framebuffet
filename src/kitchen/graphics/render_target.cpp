#include "render_target.hpp"

namespace fb::graphics::render_target {

//
// RenderTargetView.
//

auto RenderTargetView::create(const RenderTargetViewDesc& desc) -> void {
    _desc = desc;
}

auto RenderTargetView::transition_to_render_target(GpuCommandList& cmd) -> void {
    if (_desc.sample_count > 1) {
        for (auto& attachment : _desc.colors) {
            if (attachment.ms_color) {
                attachment.ms_color->transition(
                    cmd,
                    D3D12_BARRIER_SYNC_RENDER_TARGET,
                    D3D12_BARRIER_ACCESS_RENDER_TARGET,
                    D3D12_BARRIER_LAYOUT_RENDER_TARGET
                );
            }
        }
    } else {
        for (auto& attachment : _desc.colors) {
            if (attachment.color) {
                attachment.color->transition(
                    cmd,
                    D3D12_BARRIER_SYNC_RENDER_TARGET,
                    D3D12_BARRIER_ACCESS_RENDER_TARGET,
                    D3D12_BARRIER_LAYOUT_RENDER_TARGET
                );
            }
        }
    }
}

auto RenderTargetView::clear(GpuCommandList& cmd) -> void {
    if (_desc.sample_count > 1) {
        for (const auto& attachment : _desc.colors) {
            if (attachment.ms_color) {
                cmd.clear_rtv(attachment.ms_color->rtv_descriptor(), attachment.clear_color);
            }
        }
    } else {
        for (const auto& attachment : _desc.colors) {
            if (attachment.color) {
                cmd.clear_rtv(attachment.color->rtv_descriptor(), attachment.clear_color);
            }
        }
    }

    if (_desc.depth_stencil.depth_stencil) {
        cmd.clear_dsv(
            _desc.depth_stencil.depth_stencil->dsv_descriptor(),
            _desc.depth_stencil.clear_depth,
            _desc.depth_stencil.clear_stencil
        );
    }
}

auto RenderTargetView::transition_to_resolve(GpuCommandList& cmd) -> void {
    if (_desc.sample_count > 1) {
        for (auto& attachment : _desc.colors) {
            if (attachment.ms_color) {
                attachment.ms_color->transition(
                    cmd,
                    D3D12_BARRIER_SYNC_RESOLVE,
                    D3D12_BARRIER_ACCESS_RESOLVE_SOURCE,
                    D3D12_BARRIER_LAYOUT_RESOLVE_SOURCE
                );
            }
            if (attachment.color) {
                attachment.color->transition(
                    cmd,
                    D3D12_BARRIER_SYNC_RESOLVE,
                    D3D12_BARRIER_ACCESS_RESOLVE_DEST,
                    D3D12_BARRIER_LAYOUT_RESOLVE_DEST
                );
            }
        }
    }
}

auto RenderTargetView::resolve(GpuCommandList& cmd) -> void {
    if (_desc.sample_count > 1) {
        for (const auto& attachment : _desc.colors) {
            if (attachment.ms_color && attachment.color) {
                cmd.resolve_resource(
                    attachment.color->resource(),
                    attachment.ms_color->resource(),
                    attachment.ms_color->format()
                );
            }
        }
    }
}

auto RenderTargetView::transition_to_shader_resource(GpuCommandList& cmd) -> void {
    for (auto& attachment : _desc.colors) {
        if (attachment.color) {
            attachment.color->transition(
                cmd,
                D3D12_BARRIER_SYNC_PIXEL_SHADING,
                D3D12_BARRIER_ACCESS_SHADER_RESOURCE,
                D3D12_BARRIER_LAYOUT_DIRECT_QUEUE_SHADER_RESOURCE
            );
        }
    }
}

auto RenderTargetView::set_graphics(GpuGraphicsCommandList& cmd) -> void {
    cmd.set_viewport(0, 0, _desc.size.x, _desc.size.y);
    cmd.set_scissor(0, 0, _desc.size.x, _desc.size.y);

    uint rtv_count = 0;
    std::array<GpuDescriptor, MAX_ATTACHMENT_COUNT> rtv_descs = {};
    Option<GpuDescriptor> dsv_desc = {};

    if (_desc.sample_count > 1) {
        for (const auto& attachment : _desc.colors) {
            if (attachment.ms_color) {
                rtv_descs[rtv_count++] = attachment.ms_color->rtv_descriptor();
            }
        }
    } else {
        for (const auto& attachment : _desc.colors) {
            if (attachment.color) {
                rtv_descs[rtv_count++] = attachment.color->rtv_descriptor();
            }
        }
    }

    if (_desc.depth_stencil.depth_stencil) {
        dsv_desc = _desc.depth_stencil.depth_stencil->dsv_descriptor();
    }

    cmd.set_rtvs_dsv(std::span(rtv_descs.data(), rtv_count), dsv_desc);
}

//
// RenderTarget.
//

static auto make_clear_color(DXGI_FORMAT format, float4 color) -> D3D12_CLEAR_VALUE {
    return D3D12_CLEAR_VALUE {.Format = format, .Color = {color.x, color.y, color.z, color.w}};
}

static auto make_clear_ds(DXGI_FORMAT format, float depth, uint8_t stencil) -> D3D12_CLEAR_VALUE {
    return D3D12_CLEAR_VALUE {
        .Format = format,
        .DepthStencil = {.Depth = depth, .Stencil = stencil},
    };
}

auto RenderTarget::create(GpuDevice& device, const RenderTargetDesc& desc) -> void {
    ZoneScoped;
    DebugScope debug("Render Target");

    // Store.
    _desc = desc;

    // Color attachments.
    if (desc.sample_count > 1) {
        for (uint i = 0; i < MAX_ATTACHMENT_COUNT; ++i) {
            const auto& attachment = desc.colors[i];
            if (attachment.format == DXGI_FORMAT_UNKNOWN) {
                continue;
            }

            _ms_colors[i].create(
                device,
                GpuTextureDesc {
                    .format = attachment.format,
                    .width = desc.size.x,
                    .height = desc.size.y,
                    .sample_count = desc.sample_count,
                    .clear_value = make_clear_color(attachment.format, attachment.clear_color),
                },
                debug.with_name(std::format("Multisampled Color - {}", i))
            );

            _colors[i].create(
                device,
                GpuTextureDesc {
                    .format = attachment.format,
                    .width = desc.size.x,
                    .height = desc.size.y,
                    .mip_count = mip_count_from_size(desc.size),
                    .sample_count = 1,
                    .clear_value = make_clear_color(attachment.format, attachment.clear_color),
                },
                debug.with_name(std::format("Resolved Color - {}", i))
            );
        }
    } else {
        for (uint i = 0; i < MAX_ATTACHMENT_COUNT; ++i) {
            const auto& attachment = desc.colors[i];
            if (attachment.format == DXGI_FORMAT_UNKNOWN) {
                continue;
            }

            _colors[i].create(
                device,
                GpuTextureDesc {
                    .format = attachment.format,
                    .width = desc.size.x,
                    .height = desc.size.y,
                    .mip_count = mip_count_from_size(desc.size),
                    .sample_count = 1,
                    .clear_value = make_clear_color(attachment.format, attachment.clear_color),
                },
                debug.with_name(std::format("Color - {}", i))
            );
        }
    }

    // Depth-stencil attachment.
    if (desc.depth_stencil.format != DXGI_FORMAT_UNKNOWN) {
        _depth_stencil.create(
            device,
            GpuTextureDesc {
                .format = desc.depth_stencil.format,
                .width = desc.size.x,
                .height = desc.size.y,
                .sample_count = desc.sample_count,
                .clear_value = make_clear_ds(
                    desc.depth_stencil.format,
                    desc.depth_stencil.clear_depth,
                    desc.depth_stencil.clear_stencil
                ),
            },
            debug.with_name("Depth Stencil")
        );
    }
}

auto RenderTarget::view_desc() -> RenderTargetViewDesc {
    RenderTargetViewDesc view_desc = {};
    view_desc.size = _desc.size;
    view_desc.sample_count = _desc.sample_count;
    for (uint i = 0; i < MAX_ATTACHMENT_COUNT; ++i) {
        const auto& attachment = _desc.colors[i];
        if (attachment.format == DXGI_FORMAT_UNKNOWN) {
            continue;
        }

        if (_desc.sample_count > 1) {
            view_desc.colors[i].ms_color = &_ms_colors[i];
        }
        view_desc.colors[i].color = &_colors[i];
        view_desc.colors[i].clear_color = attachment.clear_color;
    }
    if (_desc.depth_stencil.format != DXGI_FORMAT_UNKNOWN) {
        view_desc.depth_stencil.depth_stencil = &_depth_stencil;
        view_desc.depth_stencil.clear_depth = _desc.depth_stencil.clear_depth;
        view_desc.depth_stencil.clear_stencil = _desc.depth_stencil.clear_stencil;
    }
    return view_desc;
}

} // namespace fb::graphics::render_target
