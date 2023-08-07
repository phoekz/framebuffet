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
    const GpuDevice& device,
    GpuDescriptors& descriptors,
    Uint2 size,
    Vector4 clear_color,
    std::string_view name) :
    _size(size),
    _clear_color(clear_color) {
    // Color target.
    {
        // Resource.
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

        // View.
        _color_descriptor = descriptors.rtv().alloc();
        device.create_render_target_view(_color.resource(), std::nullopt, _color_descriptor.cpu());
    }

    // Depth target.
    {
        // Resource.
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

        // View.
        _depth_descriptor = descriptors.dsv().alloc();
        device.create_depth_stencil_view(_depth.resource(), std::nullopt, _depth_descriptor.cpu());
    }
}

auto GpuRenderTargets::begin(GpuDevice& device) -> void {
    // Command list.
    auto* cmd = device.command_list();

    // Transition to be renderable.
    device.transition(
        _color.resource(),
        D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
        D3D12_RESOURCE_STATE_RENDER_TARGET);

    // Set viewport.
    D3D12_VIEWPORT viewport = {
        .TopLeftX = 0.0f,
        .TopLeftY = 0.0f,
        .Width = (float)_size.x,
        .Height = (float)_size.y,
        .MinDepth = 0.0f,
        .MaxDepth = 1.0f,
    };
    cmd->RSSetViewports(1, &viewport);

    // Set scissor rect.
    D3D12_RECT scissor = {
        .left = 0,
        .top = 0,
        .right = (LONG)_size.x,
        .bottom = (LONG)_size.y,
    };
    cmd->RSSetScissorRects(1, &scissor);

    // Set render targets.
    cmd->OMSetRenderTargets(1, _color_descriptor.cpu_ptr(), FALSE, _depth_descriptor.cpu_ptr());

    // Clear render targets.
    constexpr float DEPTH_VALUE = 1.0f;
    cmd->ClearRenderTargetView(_color_descriptor.cpu(), (const float*)&_clear_color, 0, nullptr);
    cmd->ClearDepthStencilView(
        _depth_descriptor.cpu(),
        D3D12_CLEAR_FLAG_DEPTH,
        DEPTH_VALUE,
        0,
        0,
        nullptr);
}

auto GpuRenderTargets::end(GpuDevice& device) -> void {
    // Transition to be shader readable.
    device.transition(
        _color.resource(),
        D3D12_RESOURCE_STATE_RENDER_TARGET,
        D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
}

}  // namespace fb
