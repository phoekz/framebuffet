#pragma once

#include <common/common.hpp>
#include "commands.hpp"
#include "descriptors.hpp"

namespace fb {

inline constexpr uint FRAME_COUNT = 2;

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

    auto transition_to_render_target(GpuCommandList& cmd, uint frame_index) -> void;
    auto clear_render_target(GpuCommandList& cmd, uint frame_index) -> void;
    auto set_render_target(GpuGraphicsCommandList& cmd, uint frame_index) -> void;
    auto transition_to_present(GpuCommandList& cmd, uint frame_index) -> void;
    auto present() -> void;

    auto size() const -> uint2 { return _size; }
    auto aspect_ratio() const -> float { return (float)_size.x / _size.y; }
    auto format() const -> DXGI_FORMAT { return _format; }
    auto backbuffer_index() const -> uint { return _swapchain->GetCurrentBackBufferIndex(); }

private:
    ComPtr<IDXGISwapChain4> _swapchain;
    uint2 _size = {};
    DXGI_FORMAT _format = {};
    ComPtr<ID3D12DescriptorHeap> _rtv_descriptor_heap;
    std::array<GpuDescriptor, FRAME_COUNT> _rtv_descriptors = {};
    std::array<ComPtr<ID3D12Resource2>, FRAME_COUNT> _rtvs = {};
};

} // namespace fb
