#pragma once

#include "../common.hpp"
#include "saber.hlsli"

namespace fb::demos::saber {

inline constexpr std::string_view NAME = "Saber"sv;
inline constexpr DXGI_FORMAT COLOR_FORMAT = DXGI_FORMAT_R11G11B10_FLOAT;
inline constexpr float4 COLOR_CLEAR_VALUE = {0.0f, 0.0f, 0.0f, 1.0f};
inline constexpr DXGI_FORMAT DEPTH_FORMAT = DXGI_FORMAT_D32_FLOAT;
inline constexpr float DEPTH_CLEAR_VALUE = 1.0f;
inline constexpr uint SAMPLE_COUNT = 4;

struct Parameters {
    float camera_distance = 2.0f;
    float camera_fov = rad_from_deg(45.0f);
    float camera_latitude = rad_from_deg(0.0f);
    float camera_longitude = rad_from_deg(0.0f);
    float4 saber_color_and_intensity = float4(1.0f, 0.5f, 0.125f, 8.0f);
};

struct Demo {
    Parameters parameters;
    Multibuffer<GpuBufferHostCbv<Constants>, FRAME_COUNT> constants;

    struct {
        RenderTargets render_targets;
        DebugDraw debug_draw;
        GpuPipeline pipeline;
        GpuBufferHostSrv<baked::Vertex> vertices;
        GpuBufferHostIndex<baked::Index> indices;
    } scene;

    struct {
        GpuPipeline threshold_pipeline;
        GpuPipeline downsample_pipeline;
        GpuPipeline upsample_pipeline;
        GpuTextureSrvUav downsample;
        GpuTextureSrvUav upsample;
    } compute;

    struct {
        RenderTargets render_targets;
        GpuPipeline pipeline;
    } blit;
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

} // namespace fb::demos::saber
