#pragma once

#include "../common.hpp"
#include "blit.hlsli"

namespace fb::techniques::blit {

static constexpr std::string_view NAME = "Blit"sv;

struct Parameters {
    uint tonemap = 1;
    float exposure_lg2 = 0.0f;
};

struct Technique {
    uint2 size;
    GpuDescriptor render_target;
    GpuBufferHostCbv<Constants> constants;
    GpuPipeline pipeline;
    Parameters parameters;
};

struct CreateDesc {
    const RenderTargets& render_targets;
    const Baked& baked;
    GpuDevice& device;
};

auto create(Technique& tech, const CreateDesc& desc) -> void;
auto gui(Technique& tech, const GuiDesc& desc) -> void;
auto update(Technique& tech, const UpdateDesc& desc) -> void;
auto render(Technique& tech, const RenderDesc& desc) -> void;

} // namespace fb::techniques::blit
