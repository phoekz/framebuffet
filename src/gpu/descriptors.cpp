#include "descriptors.hpp"

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

GpuDescriptorHeap::GpuDescriptorHeap(
    GpuDevice& device,
    std::string_view name,
    D3D12_DESCRIPTOR_HEAP_TYPE type,
    uint32_t capacity) :
    _type(type),
    _capacity(capacity) {
    D3D12_DESCRIPTOR_HEAP_DESC desc = {
        .Type = type,
        .NumDescriptors = _capacity,
    };
    if (is_shader_visible(type))
        desc.Flags |= D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
    _heap = device.create_descriptor_heap(
        desc,
        dx_name(name, "Descriptor Heap", heap_type_name(_type)));
    _descriptor_size = device.descriptor_size(type);
    _cpu_heap_start = _heap->GetCPUDescriptorHandleForHeapStart();
    if (is_shader_visible(type))
        _gpu_heap_start = _heap->GetGPUDescriptorHandleForHeapStart();
}

auto GpuDescriptorHeap::alloc() -> GpuDescriptorHandle {
    FB_ASSERT(_count < _capacity);
    GpuDescriptorHandle handle;
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

GpuDescriptors::GpuDescriptors(GpuDevice& device, std::string_view name) :
    _cbv_srv_uav_heap(
        device,
        name,
        D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV,
        CBV_SRV_UAV_DESCRIPTOR_CAPACITY),
    _sampler_heap(device, name, D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER, SAMPLER_DESCRIPTOR_CAPACITY),
    _rtv_heap(device, name, D3D12_DESCRIPTOR_HEAP_TYPE_RTV, RTV_DESCRIPTOR_CAPACITY),
    _dsv_heap(device, name, D3D12_DESCRIPTOR_HEAP_TYPE_DSV, DSV_DESCRIPTOR_CAPACITY) {}

}  // namespace fb