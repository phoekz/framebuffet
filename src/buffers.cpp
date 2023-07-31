#include "buffers.hpp"

#pragma warning(push)
#pragma warning(disable : 4458)  // declaration hides class member

namespace fb {

void GpuRawBuffer::create_raw(
    Dx& dx,
    uint32_t element_byte_size,
    uint32_t element_size,
    uint32_t byte_size,
    uint32_t alignment,
    DXGI_FORMAT format,
    bool host_visible,
    D3D12_RESOURCE_FLAGS resource_flags,
    D3D12_RESOURCE_STATES resource_state,
    std::string_view name) {
    auto heap_type = host_visible ? D3D12_HEAP_TYPE_UPLOAD : D3D12_HEAP_TYPE_DEFAULT;
    auto heap_properties = CD3DX12_HEAP_PROPERTIES(heap_type);
    resource_desc = CD3DX12_RESOURCE_DESC::Buffer(byte_size, resource_flags, alignment);
    FAIL_FAST_IF_FAILED(dx.device->CreateCommittedResource(
        &heap_properties,
        D3D12_HEAP_FLAG_NONE,
        &resource_desc,
        resource_state,
        nullptr,
        IID_PPV_ARGS(&resource)));
    fb::dx_set_name(resource, name);

    if (host_visible) {
        CD3DX12_RANGE read_range(0, 0);
        FAIL_FAST_IF_FAILED(resource->Map(0, &read_range, &ptr));
    }

    this->element_byte_size = element_byte_size;
    this->element_size = element_size;
    this->byte_size = byte_size;
    this->format = format;
    this->resource_state = resource_state;
}

D3D12_VERTEX_BUFFER_VIEW GpuRawBuffer::vertex_buffer_view() const {
    return D3D12_VERTEX_BUFFER_VIEW {
        .BufferLocation = resource->GetGPUVirtualAddress(),
        .SizeInBytes = byte_size,
        .StrideInBytes = element_byte_size,
    };
}

D3D12_INDEX_BUFFER_VIEW GpuRawBuffer::index_buffer_view() const {
    return D3D12_INDEX_BUFFER_VIEW {
        .BufferLocation = resource->GetGPUVirtualAddress(),
        .SizeInBytes = byte_size,
        .Format = format,
    };
}

D3D12_CONSTANT_BUFFER_VIEW_DESC GpuRawBuffer::constant_buffer_view_desc() const {
    return D3D12_CONSTANT_BUFFER_VIEW_DESC {
        .BufferLocation = resource->GetGPUVirtualAddress(),
        .SizeInBytes = byte_size,
    };
}

}  // namespace fb

#pragma warning(pop)
