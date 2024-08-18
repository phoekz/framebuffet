#pragma once

#include "../common.hpp"
#include "conras.hlsli"

namespace fb::demos::conras {

inline constexpr std::string_view NAME = "Conras"sv;
inline constexpr ColorAttachmentDescs COLOR_ATTACHMENTS = {ColorAttachmentDesc {
    .format = DXGI_FORMAT_R8G8B8A8_UNORM,
    .clear_color = {0.0f, 0.0f, 0.0f, 1.0f},
}};
inline constexpr DepthStencilAttachmentDesc DEPTH_STENCIL_ATTACHMENT = {
    .format = DXGI_FORMAT_D32_FLOAT,
    .clear_depth = 1.0f,
    .clear_stencil = 0,
};
inline constexpr uint SAMPLE_COUNT = 4;

struct Parameters {
    ConservativeRasterization conservative_rasterization = ConservativeRasterization::Under;
};

struct Demo {
    Parameters parameters;
    RenderTarget render_target;
    RenderTargetView render_target_view;
    DebugDraw debug_draw;
    Multibuffer<GpuBufferHostCbv<Constants>, FRAME_COUNT> constants;

    GpuBufferDeviceIndex<uint16_t> star_indices;
    GpuBufferDeviceSrvUav<uint3> raster_buffer;
    uint2 raster_buffer_size;
    GpuPipeline clear_pipeline;
    std::array<GpuPipeline, 2> raster_pipelines;
    GpuPipeline display_pipeline;
};

struct CreateDesc {
    const Baked& baked;
    GpuDevice& device;
};

auto create(Demo& demo, const CreateDesc& desc) -> void;
auto gui(Demo& demo, const GuiDesc& desc) -> void;
auto update(Demo& demo, const UpdateDesc& desc) -> void;
auto render(Demo& demo, const RenderDesc& desc) -> void;

template<Archive A>
auto archive(Demo& demo, A& arc) -> void {
    arc & demo.parameters;
}

} // namespace fb::demos::conras
