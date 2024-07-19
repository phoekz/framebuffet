#pragma once

#include "../common.hpp"
#include "conras.hlsli"

namespace fb::demos::conras {

inline constexpr std::string_view NAME = "Conras"sv;
inline constexpr DXGI_FORMAT COLOR_FORMAT = DXGI_FORMAT_R8G8B8A8_UNORM;
inline constexpr float4 COLOR_CLEAR_VALUE = {0.0f, 0.0f, 0.0f, 1.0f};
inline constexpr DXGI_FORMAT DEPTH_FORMAT = DXGI_FORMAT_D32_FLOAT;
inline constexpr float DEPTH_CLEAR_VALUE = 1.0f;
inline constexpr uint SAMPLE_COUNT = 4;

struct Parameters {
    ConservativeRasterization conservative_rasterization = ConservativeRasterization::Off;
};

struct Demo {
    Parameters parameters;
    RenderTargets render_targets;
    DebugDraw debug_draw;
    Multibuffer<GpuBufferHostCbv<Constants>, FRAME_COUNT> constants;

    GpuBufferHostIndex<uint16_t> star_indices;
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
