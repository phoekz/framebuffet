#pragma once

#include "../common.hpp"
#include "cards.hlsli"
#include <kitchen/graphics/spd.hlsli>

namespace fb::demos::cards {

inline constexpr std::string_view NAME = "Cards"sv;
inline constexpr DXGI_FORMAT COLOR_FORMAT = DXGI_FORMAT_R8G8B8A8_UNORM;
inline constexpr float4 COLOR_CLEAR_VALUE = {0.0f, 0.0f, 0.0f, 1.0f};
inline constexpr DXGI_FORMAT DEPTH_FORMAT = DXGI_FORMAT_D32_FLOAT;
inline constexpr float DEPTH_CLEAR_VALUE = 1.0f;
inline constexpr uint SAMPLE_COUNT = 1;

inline constexpr uint CARD_COUNT = 10;
inline constexpr uint CARD_GRID_COLUMNS = 4;

struct Parameters {
    std::array<uint, CARD_COUNT> card_indirect_indices;
};

struct CardDescriptors {
    uint src;
    uint mid;
    uint dst_begin;
};

struct Demo {
    Parameters parameters;
    GpuPipeline pipeline;
    Multibuffer<GpuBufferHostCbv<Constants>, FRAME_COUNT> constants;
    Multibuffer<GpuBufferHostSrv<Card>, FRAME_COUNT> cards;
    GpuBufferHostSrv<Vertex> vertices;
    GpuBufferHostIndex<uint16_t> indices;
    std::array<std::string_view, CARD_COUNT> card_names;
    std::array<CardDescriptors, CARD_COUNT> card_descriptors;

    GpuBufferHostCbv<spd::Constants> spd_constants;
    GpuBufferDeviceUav<spd::Atomics> spd_atomics;
    GpuPipeline spd_pipeline;
    uint3 spd_dispatch;
};

struct CardDesc {
    std::string_view name;
    ConstRef<RenderTargets> render_targets;
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
