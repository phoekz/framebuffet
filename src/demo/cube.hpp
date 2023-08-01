#pragma once

#include "../utils.hpp"
#include "../maths.hpp"
#include "../dx12.hpp"
#include "../buffers.hpp"
#include "../gltf.hpp"

namespace fb::cube {

struct Constants {
    Matrix transform;
    float pad[48];
};

struct UpdateParams {
    float aspect_ratio;
    float elapsed_time;
};

struct Demo {
    Demo(Dx& dx);
    void update(const UpdateParams& params);
    void render(Dx& dx);

    ComPtr<ID3D12RootSignature> root_signature;
    ComPtr<ID3D12PipelineState> pipeline_state;
    ComPtr<ID3D12DescriptorHeap> descriptor_heap;

    Constants constants;
    GpuBuffer<Constants> constant_buffer;
    D3D12_GPU_DESCRIPTOR_HANDLE constant_buffer_descriptor;
    GpuBuffer<GltfVertex> vertex_buffer;
    GpuBuffer<GltfIndex> index_buffer;

    ComPtr<ID3D12Resource> texture;
    D3D12_GPU_DESCRIPTOR_HANDLE texture_descriptor;

    ComPtr<ID3D12Resource> color_target;
    ComPtr<ID3D12DescriptorHeap> color_target_descriptor_heap;
    D3D12_CPU_DESCRIPTOR_HANDLE color_target_descriptor;

    ComPtr<ID3D12Resource> depth_target;
    ComPtr<ID3D12DescriptorHeap> depth_target_descriptor_heap;
    D3D12_CPU_DESCRIPTOR_HANDLE depth_target_descriptor;
};

}  // namespace fb::cube
