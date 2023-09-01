#pragma once

#include "../common.hpp"
#include "cfr.hlsli"

namespace fb::techniques::cfr {

static constexpr std::string_view NAME = "CFR"sv;

struct CreateDesc {
    const RenderTargets& render_targets;
    const Baked& baked;
    GpuDevice& device;
    GpuDescriptor rect_texture;
};

struct GpuCommandsDesc {
    GpuCommandList& cmd;
};

struct Technique {
    GpuDescriptor rect_texture;
    GpuTextureSrvUavCube cube_texture;
    GpuBufferHostCbv<Constants> constants;
    GpuPipeline pipeline;
    bool done = false;
};

auto create(Technique& tech, const CreateDesc& desc) -> void;
auto gui(Technique& tech, const GuiDesc& desc) -> void;
auto update(Technique& tech, const UpdateDesc& desc) -> void;
auto gpu_commands(Technique& tech, const GpuCommandsDesc& desc) -> void;

} // namespace fb::techniques::cfr
