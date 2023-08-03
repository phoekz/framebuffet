#include "render_targets.hpp"

namespace fb {

GpuRenderTargets::GpuRenderTargets(
    Dx& dx,
    GpuDescriptors& descriptors,
    uint32_t width,
    uint32_t height,
    Vector4 clear_color,
    std::string_view name) :
    _width(width),
    _height(height),
    _clear_color(clear_color) {
    // Color target.
    {
        // Resource.
        CD3DX12_HEAP_PROPERTIES heap_properties(D3D12_HEAP_TYPE_DEFAULT);
        CD3DX12_RESOURCE_DESC resource_desc = CD3DX12_RESOURCE_DESC::Tex2D(
            COLOR_FORMAT,
            _width,
            _height,
            1,
            1,
            SAMPLE_COUNT,
            0,
            D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET);
        D3D12_CLEAR_VALUE clear_value;
        clear_value.Format = COLOR_FORMAT;
        memcpy(clear_value.Color, &_clear_color, sizeof(_clear_color));
        FAIL_FAST_IF_FAILED(dx.device->CreateCommittedResource(
            &heap_properties,
            D3D12_HEAP_FLAG_NONE,
            &resource_desc,
            D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
            &clear_value,
            IID_PPV_ARGS(&_color)));

        // Debug.
        dx_set_name(_color, dx_name(name, "Color Target"));

        // View.
        _color_descriptor = descriptors.rtv().alloc();
        dx.device->CreateRenderTargetView(_color.get(), nullptr, _color_descriptor.cpu());
    }

    // Depth target.
    {
        // Resource.
        CD3DX12_HEAP_PROPERTIES heap_properties(D3D12_HEAP_TYPE_DEFAULT);
        CD3DX12_RESOURCE_DESC resource_desc = CD3DX12_RESOURCE_DESC::Tex2D(
            DEPTH_FORMAT,
            _width,
            _height,
            1,
            1,
            SAMPLE_COUNT,
            0,
            D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL);
        D3D12_CLEAR_VALUE clear_value = {
            .Format = DEPTH_FORMAT,
            .DepthStencil = {.Depth = 1.0f, .Stencil = 0}};
        FAIL_FAST_IF_FAILED(dx.device->CreateCommittedResource(
            &heap_properties,
            D3D12_HEAP_FLAG_NONE,
            &resource_desc,
            D3D12_RESOURCE_STATE_DEPTH_WRITE,
            &clear_value,
            IID_PPV_ARGS(&_depth)));

        // Debug.
        dx_set_name(_depth, dx_name(name, "Depth Target"));

        // View.
        _depth_descriptor = descriptors.dsv().alloc();
        dx.device->CreateDepthStencilView(_depth.get(), nullptr, _depth_descriptor.cpu());
    }
}

auto GpuRenderTargets::begin(Dx& dx) -> void {
    // Command list.
    auto* cmd = dx.command_list.get();

    // Transition to be renderable.
    dx.transition(
        _color,
        D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
        D3D12_RESOURCE_STATE_RENDER_TARGET);

    // Set viewport.
    D3D12_VIEWPORT viewport = {
        .TopLeftX = 0.0f,
        .TopLeftY = 0.0f,
        .Width = (float)_width,
        .Height = (float)_height,
        .MinDepth = 0.0f,
        .MaxDepth = 1.0f,
    };
    cmd->RSSetViewports(1, &viewport);

    // Set scissor rect.
    D3D12_RECT scissor = {
        .left = 0,
        .top = 0,
        .right = (LONG)_width,
        .bottom = (LONG)_height,
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

auto GpuRenderTargets::end(Dx& dx) -> void {
    // Transition to be shader readable.
    dx.transition(
        _color,
        D3D12_RESOURCE_STATE_RENDER_TARGET,
        D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
}

}  // namespace fb
