#pragma once

#include "../common.hpp"
#include "cards.hlsli"
#include <kitchen/graphics/spd.hlsli>

namespace fb::demos::cards {

inline constexpr std::string_view NAME = "Cards"sv;

inline constexpr uint CARD_COUNT = 10;
inline constexpr uint CARD_GRID_COLUMNS = 4;

struct Parameters {
    std::array<uint, CARD_COUNT> card_indirect_indices = {};
    float zoom_factor = 1.0f / 4.0f;
    float lod_bias = 8.0f;
};

struct CardDescriptors {
    uint src;
    uint mid;
    uint dst_begin;
};

struct Demo {
    Parameters parameters;
    GpuPipeline background_pipeline;
    GpuPipeline draw_pipeline;
    Multibuffer<GpuBufferHostCbv<Constants>, FRAME_COUNT> constants;
    Multibuffer<GpuBufferHostSrv<Card>, FRAME_COUNT> cards;
    GpuBufferDeviceSrv<Vertex> vertices;
    GpuBufferDeviceIndex<uint16_t> indices;
    std::array<std::string_view, CARD_COUNT> card_names;
    std::array<CardDescriptors, CARD_COUNT> card_descriptors;

    GpuBufferHostCbv<spd::Constants> spd_constants;
    GpuBufferDeviceUav<spd::Atomics> spd_atomics;
    GpuPipeline spd_pipeline;
    uint3 spd_dispatch;
};

struct CardDesc {
    std::string_view name;
    ConstRef<RenderTargetView> render_target_view;
};

struct CreateDesc {
    const Baked& baked;
    GpuDevice& device;
    std::array<CardDesc, CARD_COUNT> cards;
};

auto create(Demo& demo, const CreateDesc& desc) -> void;
auto gui(Demo& demo, const GuiDesc& desc) -> void;
auto update(Demo& demo, const UpdateDesc& desc) -> void;
auto render(Demo& demo, const RenderDesc& desc) -> void;

template<Archive A>
auto archive(Demo& demo, A& arc) -> void {
    arc & demo.parameters;
}

} // namespace fb::demos::cards
