#pragma once

#include "demos.hpp"
#include "cards.hlsli"
#include "spd.hlsli"

namespace fb::demos::cards {

inline constexpr uint32_t CARD_COUNT = 6;
inline constexpr uint32_t CARD_GRID_COLUMNS = 3;

struct Parameters {
    std::array<uint32_t, CARD_COUNT> card_indirect_indices;
};

struct CardsDesc {
    std::array<std::reference_wrapper<RenderTargets>, CARD_COUNT> card_render_targets;
};

struct CardDescriptors {
    uint32_t src;
    uint32_t mid;
    uint32_t dst_begin;
};

class Cards {
    FB_NO_COPY_MOVE(Cards);

public:
    static constexpr std::string_view NAME = "Cards"sv;

    Cards() = default;

    auto create(GpuDevice& device, const baked::Shaders& shaders, const CardsDesc& desc) -> void;
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
    std::array<CardDescriptors, CARD_COUNT> _card_descriptors;

    GpuBufferHostCbv<spd::Constants> _spd_constants;
    GpuBufferDeviceUav<spd::Atomics> _spd_atomics;
    GpuPipeline _spd_pipeline;
    Uint3 _spd_dispatch;
};

} // namespace fb::demos::cards
