#pragma once

#include "../dx12.hpp"
#include "../maths.hpp"
#include "../utils.hpp"
#include "../buffers.hpp"

namespace fb::cards {

struct Params {
    const ComPtr<ID3D12Resource>& cube_texture;
    const ComPtr<ID3D12Resource>& rain_texture;
    const ComPtr<ID3D12Resource>& tree_texture;
};

struct Constants {
    Matrix transform;
    float pad[48];
};

struct Vertex {
    Vector2 position;
    Vector2 texcoord;
};

struct Cards {
    static constexpr const char* NAME = "Cards";

    Cards(Dx& dx, const Params& params);
    void update(const Dx& dx);
    void render(Dx& dx);

    ComPtr<ID3D12RootSignature> root_signature;
    ComPtr<ID3D12PipelineState> pipeline_state;
    ComPtr<ID3D12DescriptorHeap> descriptor_heap;

    static constexpr uint32_t NUM_32BIT_VALUES = 4;
    struct CardConstants {
        Vector2 position;
        Vector2 size;
    } card_constants[3] = {
        {{0.0f, 0.0f}, {640.0f, 400.0f}},
        {{640.0f, 0.0f}, {640.0f, 400.0f}},
        {{0.0f, 400.0f}, {640.0f, 400.0f}},
    };
    static_assert(sizeof(CardConstants) == (NUM_32BIT_VALUES * sizeof(uint32_t)));

    Constants constants;
    GpuBuffer<Constants> constant_buffer;
    GpuBuffer<Vertex> vertex_buffer;
    GpuBuffer<uint16_t> index_buffer;

    D3D12_GPU_DESCRIPTOR_HANDLE cube_texture_descriptor;
    D3D12_GPU_DESCRIPTOR_HANDLE rain_texture_descriptor;
    D3D12_GPU_DESCRIPTOR_HANDLE tree_texture_descriptor;
};

}  // namespace fb::cards
