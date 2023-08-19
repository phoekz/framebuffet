#pragma once

#include "demos.hpp"
#include "cards.hlsli"

namespace fb::demos::cards {

inline constexpr uint32_t CARD_COUNT = 6;
inline constexpr uint32_t CARD_GRID_COLUMNS = 3;

struct Parameters {
    std::array<uint32_t, CARD_COUNT> card_indirect_indices;
};

struct CardsDesc {
    std::array<GpuDescriptor, CARD_COUNT> card_descriptors;
};

class Cards {
public:
    static constexpr std::string_view NAME = "Cards"sv;

    Cards(GpuDevice& device, const baked::Shaders& shaders, const CardsDesc& desc);
    auto gui(const demos::GuiDesc& desc) -> void;
    auto update(const GpuDevice& device) -> void;
    auto render(GpuDevice& device, GpuCommandList& cmd) -> void;

    template<Archive A>
    auto archive(A& arc) -> void {
        arc& _parameters;
    }

private:
    Parameters _parameters;
    GpuPipeline _pipeline;
    GpuBufferHostCbv<Constants> _constants;
    GpuBufferHostSrv<Card> _cards;
    GpuBufferHostSrv<Vertex> _vertices;
    GpuBufferHostIndex<uint16_t> _indices;
    std::array<GpuDescriptor, CARD_COUNT> _card_texture_descriptors;
};

} // namespace fb::demos::cards
