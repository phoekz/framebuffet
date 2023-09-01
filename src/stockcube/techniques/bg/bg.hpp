#pragma once

#include "../common.hpp"
#include "bg.hlsli"

namespace fb::techniques::bg {

static constexpr std::string_view NAME = "BG"sv;

struct Parameters {
    float roughness = 0.0f;
};

struct Technique {
    GpuDescriptor rad_texture;
    uint rad_texture_mip_count;
    GpuBufferHostCbv<Constants> constants;
    GpuBufferHostSrv<baked::Vertex> vertices;
    GpuBufferHostIndex<baked::Index> indices;
    GpuPipeline pipeline;
    Parameters parameters;
};

struct CreateDesc {
    const RenderTargets& render_targets;
    const Baked& baked;
    GpuDevice& device;
    GpuDescriptor rad_texture;
    uint rad_texture_mip_count;
};

auto create(Technique& tech, const CreateDesc& desc) -> void;
auto gui(Technique& tech, const GuiDesc& desc) -> void;
auto update(Technique& tech, const UpdateDesc& desc) -> void;
auto render(Technique& tech, const RenderDesc& desc) -> void;

} // namespace fb::techniques::bg
