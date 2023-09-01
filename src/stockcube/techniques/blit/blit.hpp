#pragma once

#include "../common.hpp"
#include "blit.hlsli"

namespace fb::techniques::blit {

static constexpr std::string_view NAME = "Blit"sv;

struct CreateDesc {
    const RenderTargets& render_targets;
    const Baked& baked;
    GpuDevice& device;
};

struct GpuCommandsDesc {
    GpuDevice& device;
    GpuCommandList& cmd;
};

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

auto create(Technique& tech, const CreateDesc& desc) -> void;
auto gui(Technique& tech, const GuiDesc& desc) -> void;
auto update(Technique& tech, const UpdateDesc& desc) -> void;
auto gpu_commands(Technique& tech, const GpuCommandsDesc& desc) -> void;

} // namespace fb::techniques::blit
