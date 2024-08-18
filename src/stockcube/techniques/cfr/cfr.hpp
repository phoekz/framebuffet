#pragma once

#include "../common.hpp"
#include "cfr.hlsli"

namespace fb::techniques::cfr {

static constexpr std::string_view NAME = "CFR"sv;

struct Technique {
    GpuDescriptor rect_texture;
    GpuTextureSrvUavCube cube_texture;
    GpuBufferHostCbv<Constants> constants;
    GpuPipeline pipeline;
    bool done = false;
};

struct CreateDesc {
    const RenderTargetView& render_target_view;
    const Baked& baked;
    GpuDevice& device;
    GpuDescriptor rect_texture;
};

auto create(Technique& tech, const CreateDesc& desc) -> void;
auto gui(Technique& tech, const GuiDesc& desc) -> void;
auto update(Technique& tech, const UpdateDesc& desc) -> void;
auto render(Technique& tech, const RenderDesc& desc) -> void;

} // namespace fb::techniques::cfr
