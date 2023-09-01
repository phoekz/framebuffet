#pragma once

#include "../common.hpp"
#include "rad.hlsli"

namespace fb::techniques::rad {

static constexpr std::string_view NAME = "RAD"sv;

struct Technique {
    std::string_view texture_name;
    GpuDescriptor cube_texture;
    GpuTextureUavCube acc_texture;
    GpuTextureSrvUavCube div_texture;
    GpuBufferHostReadback<std::byte> readback;
    GpuBufferHostCbv<Constants> constants;
    GpuPipeline acc_pipeline;
    GpuPipeline div_pipeline;
    uint dispatch_id;
    uint dispatch_count;
    uint sample_count_lg2;
    Delayed delayed_save;
};

struct CreateDesc {
    const RenderTargets& render_targets;
    const Baked& baked;
    GpuDevice& device;
    GpuDescriptor cube_texture;
    std::string_view texture_name;
};

auto create(Technique& tech, const CreateDesc& desc) -> void;
auto gui(Technique& tech, const GuiDesc& desc) -> void;
auto update(Technique& tech, const UpdateDesc& desc) -> void;
auto render(Technique& tech, const RenderDesc& desc) -> void;

} // namespace fb::techniques::rad
