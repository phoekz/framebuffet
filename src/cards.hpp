#pragma once

#include <gpu/gpu.hpp>

#include "maths.hpp"
#include "utils.hpp"

namespace fb::cards {

static constexpr uint32_t CARD_COUNT = 3;

struct Params {
    std::array<std::reference_wrapper<const ComPtr<ID3D12Resource>>, CARD_COUNT> card_textures;
};

struct Constants {
    Matrix transform;
    float pad[48];
};

struct Card {
    Vector2 position;
    Vector2 size;
};

struct Vertex {
    Vector2 position;
    Vector2 texcoord;
};

struct Cards {
    static constexpr std::string_view NAME = "Cards"sv;

    Cards(Dx& dx, const Params& params);
    void update(const Dx& dx);
    void render(Dx& dx);

    GpuRootSignature root_signature;
    GpuDescriptors descriptors;
    GpuSamplers samplers;

    ComPtr<ID3D12PipelineState> pipeline_state;

    GpuBufferHostCbv<Constants> constant_buffer;
    GpuDescriptorHandle constant_buffer_descriptor;

    GpuBufferHostSrv<Card> card_buffer;
    GpuDescriptorHandle card_buffer_descriptor;

    GpuBufferHostSrv<Vertex> vertex_buffer;
    GpuDescriptorHandle vertex_buffer_descriptor;

    GpuBufferHostIndex<uint16_t> index_buffer;

    std::array<GpuDescriptorHandle, CARD_COUNT> card_texture_descriptors;
};

}  // namespace fb::cards
