#pragma once

#include "../common.hpp"
#include "screen.hlsli"

namespace fb::techniques::screen {

static constexpr std::string_view NAME = "Screen"sv;

struct CreateDesc {
    const RenderTargets& render_targets;
    const Baked& baked;
    GpuDevice& device;
    GpuDescriptor rect_texture;
    GpuDescriptor cube_texture;
    GpuDescriptor lut_texture;
    GpuDescriptor irr_texture;
    GpuDescriptor rad_texture;
    uint32_t rad_texture_mip_count;
};

struct GpuCommandsDesc {
    GpuCommandList& cmd;
};

struct Technique {
    GpuPipeline pipeline;
    GpuBufferHostCbv<Constants> constants;
    GpuBufferHostSrv<Vertex> vertices;
    GpuBufferHostIndex<uint32_t> indices;
    GpuBufferHostSrv<Instance> instances;
};

auto create(Technique& tech, const CreateDesc& desc) -> void;
auto gui(Technique& tech, const GuiDesc& desc) -> void;
auto update(Technique& tech, const UpdateDesc& desc) -> void;
auto gpu_commands(Technique& tech, const GpuCommandsDesc& desc) -> void;

} // namespace fb::techniques::screen
