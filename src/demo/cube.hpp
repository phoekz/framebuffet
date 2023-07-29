#pragma once

#include "../utils.hpp"
#include "../maths.hpp"
#include "../dx12.hpp"
#include <d3dx12/d3dx12.h>

namespace fb::cube {

struct UpdateParams {
    float aspect_ratio;
    float elapsed_time;
};

struct Demo {
    Demo(Dx& dx);
    void update(const UpdateParams& params);
    void render(ID3D12GraphicsCommandList9* cmd);

    ComPtr<ID3D12RootSignature> root_signature;
    ComPtr<ID3D12PipelineState> pipeline_state;
    ComPtr<ID3D12DescriptorHeap> descriptor_heap;

    struct ConstantBuffer {
        fb::Mat4x4 transform;
        float padding[48];
    } constant_buffer_data = {};
    static_assert(
        (sizeof(ConstantBuffer) % 256) == 0,
        "Constant Buffer size must be 256-byte aligned");
    ConstantBuffer* constant_buffer_ptr = nullptr;
    ComPtr<ID3D12Resource> constant_buffer;
    D3D12_GPU_DESCRIPTOR_HANDLE constant_buffer_descriptor;

    ComPtr<ID3D12Resource> vertex_buffer;
    D3D12_VERTEX_BUFFER_VIEW vertex_buffer_view;

    uint32_t index_count = 0;
    ComPtr<ID3D12Resource> index_buffer;
    D3D12_INDEX_BUFFER_VIEW index_buffer_view;

    ComPtr<ID3D12Resource> texture;
    D3D12_GPU_DESCRIPTOR_HANDLE texture_descriptor;
};

}  // namespace fb::cube
