#pragma once

#include "../utils.hpp"
#include "../maths.hpp"
#include "../dx12.hpp"
#include "../buffers.hpp"
#include <d3dx12/d3dx12.h>

namespace fb::rain {

constexpr uint32_t PARTICLE_COUNT = 4 * 1024;
constexpr uint32_t DISPATCH_SIZE = 128;
constexpr uint32_t DISPATCH_COUNT = PARTICLE_COUNT / DISPATCH_SIZE;

struct UpdateParams {
    float aspect_ratio;
    float delta_time;
};

struct Demo {
    Demo(Dx& dx);
    void update(const UpdateParams& params);
    void render(Dx& dx);

    struct Particle {
        Vector3 position;
    };
    static_assert(sizeof(Particle) == 12);
    GpuTypedBuffer<Particle> particle_buffer;

    struct {
        ComPtr<ID3D12RootSignature> root_signature;
        ComPtr<ID3D12PipelineState> pipeline_state;
        ComPtr<ID3D12DescriptorHeap> descriptor_heap;

        struct {
            float delta_time = 0.0f;
            float speed = 0.2f;
        } constants;
    } compute;

    struct {
        ComPtr<ID3D12RootSignature> root_signature;
        ComPtr<ID3D12PipelineState> pipeline_state;
        D3D12_VERTEX_BUFFER_VIEW vertex_buffer_view;

        Matrix transform;
    } draw;

    ComPtr<ID3D12Resource> color_target;
    ComPtr<ID3D12DescriptorHeap> color_target_descriptor_heap;
    D3D12_CPU_DESCRIPTOR_HANDLE color_target_descriptor;

    ComPtr<ID3D12Resource> depth_target;
    ComPtr<ID3D12DescriptorHeap> depth_target_descriptor_heap;
    D3D12_CPU_DESCRIPTOR_HANDLE depth_target_descriptor;
};

}  // namespace fb::rain
