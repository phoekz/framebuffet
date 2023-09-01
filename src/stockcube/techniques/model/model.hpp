#pragma once

#include "../common.hpp"
#include "model.hlsli"

namespace fb::techniques::model {

static constexpr std::string_view NAME = "Model"sv;

struct CreateDesc {
    const RenderTargets& render_targets;
    const Baked& baked;
    GpuDevice& device;
    GpuDescriptor lut_texture;
    GpuDescriptor irr_texture;
    GpuDescriptor rad_texture;
    uint32_t rad_texture_mip_count;
};

struct GpuCommandsDesc {
    GpuCommandList& cmd;
};

struct Parameters {
    float metallic = 0.0f;
    float roughness = 0.0f;
};

struct Technique {
    GpuDescriptor lut_texture;
    GpuDescriptor irr_texture;
    GpuDescriptor rad_texture;
    uint32_t rad_texture_mip_count;
    GpuBufferHostCbv<Constants> constants;
    GpuBufferHostSrv<baked::Vertex> vertices;
    GpuBufferHostIndex<baked::Index> indices;
    GpuPipeline pipeline;
    Parameters parameters;
};

auto create(Technique& tech, const CreateDesc& desc) -> void;
auto gui(Technique& tech, const GuiDesc& desc) -> void;
auto update(Technique& tech, const UpdateDesc& desc) -> void;
auto gpu_commands(Technique& tech, const GpuCommandsDesc& desc) -> void;

} // namespace fb::techniques::model
