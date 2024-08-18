#pragma once

#include "../common.hpp"
#include "screen.hlsli"

namespace fb::techniques::screen {

static constexpr std::string_view NAME = "Screen"sv;

struct Technique {
    GpuPipeline pipeline;
    GpuBufferHostCbv<Constants> constants;
    GpuBufferDeviceSrv<Vertex> vertices;
    GpuBufferDeviceIndex<uint> indices;
    GpuBufferDeviceSrv<Instance> instances;
};

struct CreateDesc {
    const RenderTargetView& render_target_view;
    const Baked& baked;
    GpuDevice& device;
    GpuDescriptor rect_texture;
    GpuDescriptor cube_texture;
    GpuDescriptor lut_texture;
    GpuDescriptor irr_texture;
    GpuDescriptor rad_texture;
    uint rad_texture_mip_count;
};

auto create(Technique& tech, const CreateDesc& desc) -> void;
auto gui(Technique& tech, const GuiDesc& desc) -> void;
auto update(Technique& tech, const UpdateDesc& desc) -> void;
auto render(Technique& tech, const RenderDesc& desc) -> void;

} // namespace fb::techniques::screen
