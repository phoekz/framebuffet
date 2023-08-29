#pragma once

#include "../pch.hpp"
#include "commands.hpp"
#include "descriptors.hpp"

namespace fb {

inline constexpr uint32_t FRAME_COUNT = 2;

class GpuDevice;
class Window;

class GpuSwapchain {
    FB_NO_COPY_MOVE(GpuSwapchain);

public:
    GpuSwapchain() = default;

    auto create(
        const ComPtr<IDXGIFactory7>& factory,
        const ComPtr<ID3D12Device>& device,
        const ComPtr<ID3D12CommandQueue>& command_queue,
        const Window& window,
        GpuDescriptors& descriptors
    ) -> void;

    auto transition_to_render_target(GpuCommandList& cmd, uint32_t frame_index) -> void;
    auto clear_render_target(GpuCommandList& cmd, uint32_t frame_index) -> void;
    auto set_render_target(GpuCommandList& cmd, uint32_t frame_index) -> void;
    auto transition_to_present(GpuCommandList& cmd, uint32_t frame_index) -> void;
    auto present() -> void;

    auto size() const -> Uint2 { return _swapchain_size; }
    auto format() const -> DXGI_FORMAT { return _swapchain_format; }
    auto backbuffer_index() const -> uint32_t { return _swapchain->GetCurrentBackBufferIndex(); }

private:
    ComPtr<IDXGISwapChain4> _swapchain;
    Uint2 _swapchain_size = {};
    DXGI_FORMAT _swapchain_format = {};
    ComPtr<ID3D12DescriptorHeap> _rtv_descriptor_heap;
    std::array<GpuDescriptor, FRAME_COUNT> _rtv_descriptors = {};
    std::array<ComPtr<ID3D12Resource>, FRAME_COUNT> _rtvs = {};
};

} // namespace fb
