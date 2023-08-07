#pragma once

#include <gpu/gpu.hpp>

#include "maths.hpp"
#include "utils.hpp"

namespace fb::cards {

static constexpr uint32_t CARD_COUNT = 4;

struct Params {
    std::array<GpuDescriptorHandle, CARD_COUNT> card_texture_descriptors;
};

struct Constants {
    Matrix transform;
    float pad[48] = {};
};

struct Card {
    Vector2 position;
    Vector2 size;
};

struct Vertex {
    Vector2 position;
    Vector2 texcoord;
};

class Cards {
  public:
    static constexpr std::string_view NAME = "Cards"sv;

    Cards(GpuDevice& device, const Params& params);
    auto update(const GpuDevice& device) -> void;
    auto render(GpuDevice& device) -> void;

  private:
    ComPtr<ID3D12PipelineState> _pipeline_state;
    GpuBufferHostCbv<Constants> _constant_buffer;
    GpuBufferHostSrv<Card> _card_buffer;
    GpuBufferHostSrv<Vertex> _vertex_buffer;
    GpuBufferHostIndex<uint16_t> _index_buffer;
    std::array<GpuDescriptorHandle, CARD_COUNT> _card_texture_descriptors;
};

}  // namespace fb::cards
