#pragma once

#include "demos.hpp"

namespace fb::demos::cards {

inline constexpr uint32_t CARD_COUNT = 6;
inline constexpr uint32_t CARD_GRID_COLUMNS = 3;

struct Params {
    std::array<GpuDescriptor, CARD_COUNT> card_texture_descriptors;
};

struct Constants {
    Float4x4 transform;
    float pad[48] = {};
};

struct Card {
    Float2 position;
    Float2 size;
};

struct Vertex {
    Float2 position;
    Float2 texcoord;
};

class Cards {
  public:
    static constexpr std::string_view NAME = "Cards"sv;

    Cards(GpuDevice& device, const baked::Shaders& shaders, const Params& params);
    auto gui(const demos::GuiDesc& desc) -> void;
    auto update(const GpuDevice& device) -> void;
    auto render(GpuDevice& device, GpuCommandList& cmd) -> void;

  private:
    GpuPipeline _pipeline;
    GpuBufferHostCbv<Constants> _constants;
    GpuBufferHostSrv<Card> _cards;
    GpuBufferHostSrv<Vertex> _vertices;
    GpuBufferHostIndex<uint16_t> _indices;
    std::array<GpuDescriptor, CARD_COUNT> _card_texture_descriptors;
    std::array<uint32_t, CARD_COUNT> _card_indirect_indices;
};

} // namespace fb::demos::cards
