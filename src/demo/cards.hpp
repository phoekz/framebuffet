#pragma once

#include "../dx12.hpp"
#include "../maths.hpp"
#include "../utils.hpp"
#include "../buffers.hpp"

namespace fb::cards {

struct Params {
    const ComPtr<ID3D12Resource>& cube_texture;
    const ComPtr<ID3D12Resource>& rain_texture;
};

struct Constants {
    Mat4x4 transform;
    float pad[48];
};

struct Vertex {
    Vec2 position;
    Vec2 texcoord;
};

struct Cards {
    Cards(Dx& dx, const Params& params);
    void update(const Dx& dx);
    void render(Dx& dx);

    ComPtr<ID3D12RootSignature> root_signature;
    ComPtr<ID3D12PipelineState> pipeline_state;
    ComPtr<ID3D12DescriptorHeap> descriptor_heap;

    static constexpr uint32_t NUM_32BIT_VALUES = 4;
    struct CardConstants {
        fb::Vec2 position;
        fb::Vec2 size;
    } card_constants[2] = {
        {{0.0f, 0.0f}, {640.0f, 400.0f}},
        {{640.0f, 0.0f}, {640.0f, 400.0f}},
    };
    static_assert(sizeof(CardConstants) == (NUM_32BIT_VALUES * sizeof(uint32_t)));

    Constants constants;
    GpuBuffer<Constants> constant_buffer;
    GpuBuffer<Vertex> vertex_buffer;
    GpuBuffer<uint16_t> index_buffer;

    D3D12_GPU_DESCRIPTOR_HANDLE cube_texture_descriptor;
    D3D12_GPU_DESCRIPTOR_HANDLE rain_texture_descriptor;
};

}  // namespace fb::cards
