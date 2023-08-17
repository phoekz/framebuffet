#pragma once

#include "../pch.hpp"

namespace fb {

class GpuDevice;

class GpuDescriptor {
    friend class GpuDescriptorHeap;

  public:
    auto type() const -> D3D12_DESCRIPTOR_HEAP_TYPE { return _type; }
    auto index() const -> uint32_t { return _index; }
    auto cpu() const -> D3D12_CPU_DESCRIPTOR_HANDLE { return _cpu_handle; }
    auto cpu_ptr() const -> const D3D12_CPU_DESCRIPTOR_HANDLE* { return &_cpu_handle; }
    auto gpu() const -> D3D12_GPU_DESCRIPTOR_HANDLE { return _gpu_handle; }
    auto gpu_ptr() const -> const D3D12_GPU_DESCRIPTOR_HANDLE* { return &_gpu_handle; }

  private:
    D3D12_DESCRIPTOR_HEAP_TYPE _type = D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES;
    uint32_t _index = UINT32_MAX;
    D3D12_CPU_DESCRIPTOR_HANDLE _cpu_handle = {};
    D3D12_GPU_DESCRIPTOR_HANDLE _gpu_handle = {};
};

class GpuDescriptorHeap {
  public:
    GpuDescriptorHeap(
        GpuDevice& device,
        std::string_view name,
        D3D12_DESCRIPTOR_HEAP_TYPE type,
        uint32_t capacity);

    auto alloc() -> GpuDescriptor;
    auto type() const -> D3D12_DESCRIPTOR_HEAP_TYPE { return _type; }
    auto count() const -> uint32_t { return _count; }
    auto capacity() const -> uint32_t { return _capacity; }
    auto heap() const -> ID3D12DescriptorHeap* { return _heap.get(); }
    auto heap_ptr() const -> ID3D12DescriptorHeap* const* { return _heap.addressof(); }

  private:
    D3D12_DESCRIPTOR_HEAP_TYPE _type = D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES;
    uint32_t _descriptor_size = 0;
    uint32_t _count = 0;
    uint32_t _capacity = 0;
    ComPtr<ID3D12DescriptorHeap> _heap;
    D3D12_CPU_DESCRIPTOR_HANDLE _cpu_heap_start = {};
    D3D12_GPU_DESCRIPTOR_HANDLE _gpu_heap_start = {};
};

class GpuDescriptors {
  public:
    static constexpr uint32_t CBV_SRV_UAV_DESCRIPTOR_CAPACITY = 128;
    static constexpr uint32_t SAMPLER_DESCRIPTOR_CAPACITY = 6;
    static constexpr uint32_t RTV_DESCRIPTOR_CAPACITY = 16;
    static constexpr uint32_t DSV_DESCRIPTOR_CAPACITY = 16;

    GpuDescriptors(GpuDevice& device, std::string_view name);

    auto rtv() -> GpuDescriptorHeap& { return _rtv_heap; }
    auto dsv() -> GpuDescriptorHeap& { return _dsv_heap; }
    auto cbv_srv_uav() -> GpuDescriptorHeap& { return _cbv_srv_uav_heap; }
    auto sampler() -> GpuDescriptorHeap& { return _sampler_heap; }
    auto log_stats() -> void;

  private:
    GpuDescriptorHeap _cbv_srv_uav_heap;
    GpuDescriptorHeap _sampler_heap;
    GpuDescriptorHeap _rtv_heap;
    GpuDescriptorHeap _dsv_heap;
};

class GpuBindings {
  public:
    static constexpr uint32_t BINDINGS_CAPACITY = 16;

    GpuBindings() { _bindings.fill(UINT32_MAX); }

    auto push(uint32_t binding) -> void {
        assert(_count < BINDINGS_CAPACITY);
        _bindings[_count++] = binding;
    }
    auto push(const GpuDescriptor& handle) -> void { push(handle.index()); }
    auto count() const -> uint32_t { return _count; }
    auto capacity() -> uint32_t { return BINDINGS_CAPACITY; }
    auto ptr() const -> const uint32_t* { return _bindings.data(); }

  private:
    std::array<uint32_t, BINDINGS_CAPACITY> _bindings;
    uint32_t _count = 0;
};

}  // namespace fb
