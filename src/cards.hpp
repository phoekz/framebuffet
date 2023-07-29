#pragma once

#include "dx12.hpp"
#include "maths.hpp"
#include "utils.hpp"

namespace fb {

struct CardsParams {
    const ComPtr<ID3D12Resource>& texture;
};
struct CardsRenderParams {};

struct Cards {
    Cards(Dx& dx, const CardsParams& params);
    void update(const Dx& dx);
    void render(Dx& dx);

    ComPtr<ID3D12RootSignature> root_signature;
    ComPtr<ID3D12PipelineState> pipeline_state;
    ComPtr<ID3D12DescriptorHeap> descriptors;

    static constexpr uint32_t NUM_32BIT_VALUES = 4;
    struct CardConstants {
        fb::Vec2 position;
        fb::Vec2 size;
    } card_constants = {{0.0f, 0.0f}, {640.0f, 400.0f}};
    static_assert(sizeof(CardConstants) == (NUM_32BIT_VALUES * sizeof(uint32_t)));
    struct ConstantBuffer {
        fb::Mat4x4 transform;
        float padding[48];
    } constant_buffer_data = {};
    static_assert(
        (sizeof(ConstantBuffer) % 256) == 0,
        "Constant Buffer size must be 256-byte aligned");
    ConstantBuffer* constant_buffer_ptr = nullptr;
    ComPtr<ID3D12Resource> constant_buffer;

    ComPtr<ID3D12Resource> vertex_buffer;
    D3D12_VERTEX_BUFFER_VIEW vertex_buffer_view;

    uint32_t index_count = 0;
    ComPtr<ID3D12Resource> index_buffer;
    D3D12_INDEX_BUFFER_VIEW index_buffer_view;

    D3D12_GPU_DESCRIPTOR_HANDLE texture_descriptor;
};

}  // namespace fb
