#include "swapchain.hpp"
#include "debug.hpp"
#include "../win32/window.hpp"

namespace fb {

auto GpuSwapchain::create(
    const ComPtr<IDXGIFactory7>& factory,
    const ComPtr<ID3D12Device>& device,
    const ComPtr<ID3D12CommandQueue>& command_queue,
    const Window& window,
    GpuDescriptors& descriptors
) -> void {
    DebugScope debug("Swapchain");

    // Swapchain.
    {
        ComPtr<IDXGISwapChain1> swapchain1;
        DXGI_SWAP_CHAIN_DESC1 desc = {
            .Width = 0,
            .Height = 0,
            .Format = DXGI_FORMAT_R8G8B8A8_UNORM,
            .Stereo = FALSE,
            .SampleDesc =
                {
                    .Count = 1,
                    .Quality = 0,
                },
            .BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT,
            .BufferCount = FRAME_COUNT,
            .Scaling = DXGI_SCALING_STRETCH,
            .SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD,
            .AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED,
            .Flags = DXGI_SWAP_CHAIN_FLAG_FRAME_LATENCY_WAITABLE_OBJECT,
        };
        FB_ASSERT_HR(factory->CreateSwapChainForHwnd(
            command_queue.get(),
            window.hwnd(),
            &desc,
            nullptr,
            nullptr,
            &swapchain1
        ));
        swapchain1.query_to(&_swapchain);
    }

    // Swapchain size.
    {
        DXGI_SWAP_CHAIN_DESC1 desc;
        _swapchain->GetDesc1(&desc);
        _swapchain_size = {desc.Width, desc.Height};
    }

    // Swapchain format.
    _swapchain_format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;

    // Render target views.
    for (uint32_t i = 0; i < FRAME_COUNT; i++) {
        FB_ASSERT_HR(_swapchain->GetBuffer(i, IID_PPV_ARGS(&_rtvs[i])));
        dx_set_name(_rtvs[i], debug.with_name(std::format("Render Target {}", i)));
        const auto descriptor = descriptors.rtv().alloc();
        D3D12_RENDER_TARGET_VIEW_DESC rtv_desc = {
            .Format = _swapchain_format,
            .ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D,
        };
        device->CreateRenderTargetView(_rtvs[i].get(), &rtv_desc, descriptor.cpu());
        _rtv_descriptors[i] = descriptor;
    }
}

auto GpuSwapchain::transition_to_render_target(GpuCommandList& cmd, uint32_t frame_index) -> void {
    cmd.transition_barrier(
        _rtvs[frame_index],
        D3D12_RESOURCE_STATE_PRESENT,
        D3D12_RESOURCE_STATE_RENDER_TARGET
    );
}

auto GpuSwapchain::clear_render_target(GpuCommandList& cmd, uint32_t frame_index) -> void {
    cmd.clear_rtv(_rtv_descriptors[frame_index], Float4(0.1f, 0.1f, 0.1f, 1.0f));
}

auto GpuSwapchain::set_render_target(GpuCommandList& cmd, uint32_t frame_index) -> void {
    cmd.set_rtv_dsv(_rtv_descriptors[frame_index], std::nullopt);
}

auto GpuSwapchain::transition_to_present(GpuCommandList& cmd, uint32_t frame_index) -> void {
    cmd.transition_barrier(
        _rtvs[frame_index],
        D3D12_RESOURCE_STATE_RENDER_TARGET,
        D3D12_RESOURCE_STATE_PRESENT
    );
}

auto GpuSwapchain::present() -> void {
    FB_ASSERT_HR(_swapchain->Present(1, 0));
}

} // namespace fb
