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
    std::string_view name) {
    // Resource.
    auto heap_type = host_visible ? D3D12_HEAP_TYPE_UPLOAD : D3D12_HEAP_TYPE_DEFAULT;
    auto resource_state =
        host_visible ? D3D12_RESOURCE_STATE_GENERIC_READ : D3D12_RESOURCE_STATE_COPY_DEST;
    auto heap_properties = CD3DX12_HEAP_PROPERTIES(heap_type);
    _resource_desc = CD3DX12_RESOURCE_DESC::Buffer(byte_size, resource_flags, alignment);
    FAIL_FAST_IF_FAILED(dx.device->CreateCommittedResource(
        &heap_properties,
        D3D12_HEAP_FLAG_NONE,
        &_resource_desc,
        resource_state,
        nullptr,
        IID_PPV_ARGS(&_resource)));

    // Debug.
    dx_set_name(_resource, name);

    // Map host visible pointer.
    if (host_visible) {
        CD3DX12_RANGE read_range(0, 0);
        FAIL_FAST_IF_FAILED(_resource->Map(0, &read_range, &_raw));
    }

    // GPU address.
    _gpu_address = _resource->GetGPUVirtualAddress();

    // Save.
    this->_element_byte_size = element_byte_size;
    this->_element_size = element_size;
    this->_byte_size = byte_size;
    this->_format = format;
    this->_resource_state = resource_state;
}

auto GpuRawBuffer::vertex_buffer_view() const -> D3D12_VERTEX_BUFFER_VIEW {
    return D3D12_VERTEX_BUFFER_VIEW {
        .BufferLocation = _resource->GetGPUVirtualAddress(),
        .SizeInBytes = _byte_size,
        .StrideInBytes = _element_byte_size,
    };
}

auto GpuRawBuffer::index_buffer_view() const -> D3D12_INDEX_BUFFER_VIEW {
    return D3D12_INDEX_BUFFER_VIEW {
        .BufferLocation = _resource->GetGPUVirtualAddress(),
        .SizeInBytes = _byte_size,
        .Format = _format,
    };
}

auto GpuRawBuffer::constant_buffer_view_desc() const -> D3D12_CONSTANT_BUFFER_VIEW_DESC {
    return D3D12_CONSTANT_BUFFER_VIEW_DESC {
        .BufferLocation = _resource->GetGPUVirtualAddress(),
        .SizeInBytes = _byte_size,
    };
}

}  // namespace fb

#pragma warning(pop)
