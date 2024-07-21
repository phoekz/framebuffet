#pragma once

#include "../common.hpp"
#include "model.hlsli"

namespace fb::techniques::model {

static constexpr std::string_view NAME = "Model"sv;

struct Parameters {
    float metallic = 0.0f;
    float roughness = 0.0f;
};

struct Technique {
    GpuDescriptor lut_texture;
    GpuDescriptor irr_texture;
    GpuDescriptor rad_texture;
    uint rad_texture_mip_count;
    Multibuffer<GpuBufferHostCbv<Constants>, FRAME_COUNT> constants;
    GpuBufferDeviceSrv<baked::Vertex> vertices;
    GpuBufferDeviceIndex<baked::Index> indices;
    GpuPipeline pipeline;
    Parameters parameters;
};

struct CreateDesc {
    const RenderTargets& render_targets;
    const Baked& baked;
    GpuDevice& device;
    GpuDescriptor lut_texture;
    GpuDescriptor irr_texture;
    GpuDescriptor rad_texture;
    uint rad_texture_mip_count;
};

auto create(Technique& tech, const CreateDesc& desc) -> void;
auto gui(Technique& tech, const GuiDesc& desc) -> void;
auto update(Technique& tech, const UpdateDesc& desc) -> void;
auto render(Technique& tech, const RenderDesc& desc) -> void;

} // namespace fb::techniques::model
