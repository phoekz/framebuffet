#include "descriptors.hpp"
#include "device.hpp"
#include "debug.hpp"

namespace fb {

static auto is_shader_visible(D3D12_DESCRIPTOR_HEAP_TYPE type) -> bool {
    return type == D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV
        || type == D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER;
}

static auto heap_type_name(D3D12_DESCRIPTOR_HEAP_TYPE type) -> std::string_view {
    switch (type) {
        case D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV: return "CbvSrvUav"sv;
        case D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER: return "Sampler"sv;
        case D3D12_DESCRIPTOR_HEAP_TYPE_RTV: return "Rtv"sv;
        case D3D12_DESCRIPTOR_HEAP_TYPE_DSV: return "Dsv"sv;
        default: return "Unknown"sv;
    }
}

auto GpuDescriptorHeap::create(
    GpuDevice& device,
    std::string_view name,
    D3D12_DESCRIPTOR_HEAP_TYPE type,
    uint32_t capacity
) -> void {
    _type = type;
    _capacity = capacity;
    D3D12_DESCRIPTOR_HEAP_DESC desc = {
        .Type = type,
        .NumDescriptors = _capacity,
    };
    if (is_shader_visible(type)) {
        desc.Flags |= D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
    }
    _heap = device.create_descriptor_heap(
        desc,
        dx_name(name, "Descriptor Heap", heap_type_name(_type))
    );
    _descriptor_size = device.descriptor_size(type);
    _cpu_heap_start = _heap->GetCPUDescriptorHandleForHeapStart();
    if (is_shader_visible(type)) {
        _gpu_heap_start = _heap->GetGPUDescriptorHandleForHeapStart();
    }
}

auto GpuDescriptorHeap::alloc() -> GpuDescriptor {
    FB_ASSERT(_count < _capacity);
    GpuDescriptor handle;
    handle._type = _type;
    handle._index = _count++;
    handle._cpu_handle = _cpu_heap_start;
    handle._cpu_handle.ptr += handle._index * _descriptor_size;
    if (is_shader_visible(_type)) {
        handle._gpu_handle = _gpu_heap_start;
        handle._gpu_handle.ptr += handle._index * _descriptor_size;
    }
    return handle;
}

auto GpuDescriptors::create(GpuDevice& device, std::string_view name) -> void {
    _cbv_srv_uav_heap
        .create(device, name, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, MAX_CBV_SRV_UAV_DESCRIPTOR);
    _sampler_heap.create(device, name, D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER, MAX_SAMPLER_DESCRIPTOR);
    _rtv_heap.create(device, name, D3D12_DESCRIPTOR_HEAP_TYPE_RTV, MAX_RTV_DESCRIPTOR);
    _dsv_heap.create(device, name, D3D12_DESCRIPTOR_HEAP_TYPE_DSV, MAX_DSV_DESCRIPTOR);
}

auto GpuDescriptors::log_stats() -> void {
    FB_LOG_INFO(
        "Descriptor heaps: "
        "CbvSrvUav: {}/{} "
        "Sampler: {}/{} "
        "Rtv: {}/{} "
        "Dsv: {}/{}",
        _cbv_srv_uav_heap.count(),
        _cbv_srv_uav_heap.capacity(),
        _sampler_heap.count(),
        _sampler_heap.capacity(),
        _rtv_heap.count(),
        _rtv_heap.capacity(),
        _dsv_heap.count(),
        _dsv_heap.capacity()
    );
}

} // namespace fb
