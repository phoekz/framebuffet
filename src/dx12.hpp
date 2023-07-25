#pragma once

#include "win32.hpp"
#include <d3dx12/d3dx12.h>
#include <dxgi1_6.h>
#include <array>

namespace fb {

constexpr D3D_FEATURE_LEVEL MIN_FEATURE_LEVEL = D3D_FEATURE_LEVEL_12_2;
constexpr uint32_t FRAME_COUNT = 2;

struct Dx {
    ID3D12Device12* device = nullptr;
    ID3D12CommandQueue* command_queue = nullptr;
    std::array<ID3D12CommandAllocator*, FRAME_COUNT> command_allocators = {};
    ID3D12GraphicsCommandList9* command_list = nullptr;
    IDXGISwapChain4* swapchain = nullptr;
    ID3D12DescriptorHeap* rtv_descriptor_heap;
    std::array<D3D12_CPU_DESCRIPTOR_HANDLE, FRAME_COUNT> rtv_descriptors = {};
    std::array<ID3D12Resource*, fb::FRAME_COUNT> rtvs = {};

    uint32_t frame_index = 0;
    ID3D12Fence1* fence = nullptr;
    HANDLE fence_event = nullptr;
    std::array<uint64_t, FRAME_COUNT> fence_values = {};
};
void dx_create(Dx* dx, Window* window);
void dx_destroy(Dx* dx);

}  // namespace fb
