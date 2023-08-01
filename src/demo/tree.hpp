#pragma once

#include "../utils.hpp"
#include "../maths.hpp"
#include "../dx12.hpp"
#include "../buffers.hpp"
#include "../gltf.hpp"
#include <d3dx12/d3dx12.h>

namespace fb::tree {

constexpr uint32_t SHADOW_MAP_SIZE = 1024;

struct ShadowConstants {
    Matrix transform;
    float pad[48];
};

struct MainConstants {
    Matrix transform;
    Matrix light_transform;
    Vector3 light_direction;
    float ambient_light;
    float pad[28];
};

struct UpdateParams {
    float aspect_ratio;
    float elapsed_time;
};

struct Demo {
    static constexpr const char* NAME = "Tree";

    Demo(Dx& dx);
    void update(const UpdateParams& params);
    void render(Dx& dx);

    struct Scene {
        struct Model {
            GpuBuffer<GltfVertex> vertex_buffer;
            GpuBuffer<GltfIndex> index_buffer;
            ComPtr<ID3D12Resource> texture;
            D3D12_CPU_DESCRIPTOR_HANDLE texture_cpu_descriptor = {};
            D3D12_GPU_DESCRIPTOR_HANDLE texture_gpu_descriptor = {};
        };
        Model tree;
        Model plane;
    } scene;

    struct ShadowPass {
        static constexpr const char* NAME = "Shadow";

        ComPtr<ID3D12RootSignature> root_signature;
        ComPtr<ID3D12PipelineState> pipeline_state;

        GpuBuffer<ShadowConstants> constants;
        ComPtr<ID3D12DescriptorHeap> constants_descriptor_heap;
        D3D12_CPU_DESCRIPTOR_HANDLE constants_cpu_descriptor = {};
        D3D12_GPU_DESCRIPTOR_HANDLE constants_gpu_descriptor = {};

        ComPtr<ID3D12Resource> depth;
        ComPtr<ID3D12DescriptorHeap> depth_descriptor_heap;
        D3D12_CPU_DESCRIPTOR_HANDLE depth_cpu_descriptor = {};
        D3D12_CPU_DESCRIPTOR_HANDLE depth_srv_cpu_descriptor = {};
        D3D12_GPU_DESCRIPTOR_HANDLE depth_srv_gpu_descriptor = {};
    } shadow_pass;

    struct MainPass {
        static constexpr const char* NAME = "Main";

        ComPtr<ID3D12RootSignature> root_signature;
        ComPtr<ID3D12PipelineState> pipeline_state;
        ComPtr<ID3D12DescriptorHeap> descriptor_heap;

        GpuBuffer<MainConstants> constants;
        D3D12_CPU_DESCRIPTOR_HANDLE constants_cpu_descriptor = {};
        D3D12_GPU_DESCRIPTOR_HANDLE constants_gpu_descriptor = {};
    } main_pass;

    struct Target {
        ComPtr<ID3D12Resource> color;
        ComPtr<ID3D12DescriptorHeap> color_descriptor_heap;
        D3D12_CPU_DESCRIPTOR_HANDLE color_descriptor = {};

        ComPtr<ID3D12Resource> depth;
        ComPtr<ID3D12DescriptorHeap> depth_descriptor_heap;
        D3D12_CPU_DESCRIPTOR_HANDLE depth_descriptor = {};
    } target;
};

}  // namespace fb::tree
