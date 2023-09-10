#pragma once

#include "../pch.hpp"

namespace fb {

class GpuDevice;

class GpuDescriptor {
    friend class GpuDescriptorHeap;

public:
    auto type() const -> D3D12_DESCRIPTOR_HEAP_TYPE { return _type; }
    auto index() const -> uint { return _index; }
    auto cpu() const -> D3D12_CPU_DESCRIPTOR_HANDLE { return _cpu_handle; }
    auto cpu_ptr() const -> const D3D12_CPU_DESCRIPTOR_HANDLE* { return &_cpu_handle; }
    auto gpu() const -> D3D12_GPU_DESCRIPTOR_HANDLE { return _gpu_handle; }
    auto gpu_ptr() const -> const D3D12_GPU_DESCRIPTOR_HANDLE* { return &_gpu_handle; }

private:
    D3D12_DESCRIPTOR_HEAP_TYPE _type = D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES;
    uint _index = UINT32_MAX;
    D3D12_CPU_DESCRIPTOR_HANDLE _cpu_handle = {};
    D3D12_GPU_DESCRIPTOR_HANDLE _gpu_handle = {};
};

class GpuDescriptorHeap {
    FB_NO_COPY_MOVE(GpuDescriptorHeap);

public:
    GpuDescriptorHeap() = default;

    auto create(GpuDevice& device, D3D12_DESCRIPTOR_HEAP_TYPE type, uint capacity) -> void;
    auto alloc() -> GpuDescriptor;
    auto type() const -> D3D12_DESCRIPTOR_HEAP_TYPE { return _type; }
    auto count() const -> uint { return _count; }
    auto capacity() const -> uint { return _capacity; }
    auto heap() const -> ID3D12DescriptorHeap* { return _heap.get(); }
    auto heap_ptr() const -> ID3D12DescriptorHeap* const* { return _heap.addressof(); }

private:
    D3D12_DESCRIPTOR_HEAP_TYPE _type = D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES;
    uint _descriptor_size = 0;
    uint _count = 0;
    uint _capacity = 0;
    ComPtr<ID3D12DescriptorHeap> _heap;
    D3D12_CPU_DESCRIPTOR_HANDLE _cpu_heap_start = {};
    D3D12_GPU_DESCRIPTOR_HANDLE _gpu_heap_start = {};
};

class GpuDescriptors {
    FB_NO_COPY_MOVE(GpuDescriptors);

public:
    static constexpr uint MAX_CBV_SRV_UAV_DESCRIPTOR = 256;
    static constexpr uint MAX_SAMPLER_DESCRIPTOR = 16;
    static constexpr uint MAX_RTV_DESCRIPTOR = 16;
    static constexpr uint MAX_DSV_DESCRIPTOR = 16;

    GpuDescriptors() = default;

    auto create(GpuDevice& device) -> void;
    auto cbv_srv_uav() -> GpuDescriptorHeap& { return _cbv_srv_uav_heap; }
    auto sampler() -> GpuDescriptorHeap& { return _sampler_heap; }
    auto rtv() -> GpuDescriptorHeap& { return _rtv_heap; }
    auto dsv() -> GpuDescriptorHeap& { return _dsv_heap; }
    auto log_stats() -> void;

private:
    GpuDescriptorHeap _cbv_srv_uav_heap;
    GpuDescriptorHeap _sampler_heap;
    GpuDescriptorHeap _rtv_heap;
    GpuDescriptorHeap _dsv_heap;
};


} // namespace fb
