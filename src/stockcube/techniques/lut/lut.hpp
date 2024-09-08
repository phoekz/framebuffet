#pragma once

#include "../common.hpp"
#include "lut.hlsli"

namespace fb::techniques::lut {

static constexpr std::string_view NAME = "LUT"sv;

struct Technique {
    std::string_view texture_name;
    GpuTextureSrvUav lut_texture;
    GpuBufferHostReadback<std::byte> lut_readback;
    GpuBufferHostCbv<Constants> constants;
    GpuPipeline pipeline;
    bool done = false;
    Delayed delayed_save;
};

struct CreateDesc {
    const KcnRenderTargetView& render_target_view;
    const Baked& baked;
    GpuDevice& device;
    std::string_view texture_name;
};

auto create(Technique& tech, const CreateDesc& desc) -> void;
auto gui(Technique& tech, const GuiDesc& desc) -> void;
auto update(Technique& tech, const UpdateDesc& desc) -> void;
auto render(Technique& tech, const RenderDesc& desc) -> void;

} // namespace fb::techniques::lut
