#pragma once

#include "../common.hpp"
#include "saber.hlsli"

namespace fb::demos::saber {

inline constexpr std::string_view NAME = "Saber"sv;
inline constexpr ColorAttachmentDescs COLOR_ATTACHMENTS = {ColorAttachmentDesc {
    .format = DXGI_FORMAT_R11G11B10_FLOAT,
    .clear_color = {0.0f, 0.0f, 0.0f, 1.0f},
}};
inline constexpr DepthStencilAttachmentDesc DEPTH_STENCIL_ATTACHMENT = {
    .format = DXGI_FORMAT_D32_FLOAT,
    .clear_depth = 1.0f,
    .clear_stencil = 0,
};
inline constexpr uint SAMPLE_COUNT = 4;

inline constexpr float MAX_SABER_INTENSITY = 64.0f;

struct Parameters {
    float4 saber_color_and_intensity = float4(0.118f, 0.743f, 1.000f, 16.000f);
    uint animated = (uint)(true);
    uint tonemap = (uint)(true);
    float camera_distance = 2.0f;
    float camera_fov = rad_from_deg(45.0f);
    float camera_latitude = rad_from_deg(0.0f);
    float camera_longitude = rad_from_deg(180.0f);
};

struct Demo {
    Parameters parameters;
    Multibuffer<GpuBufferHostCbv<Constants>, FRAME_COUNT> constants;

    struct {
        RenderTarget render_target;
        RenderTargetView render_target_view;
        DebugDraw debug_draw;
        GpuPipeline pipeline;
        GpuBufferDeviceSrv<baked::Vertex> vertices;
        GpuBufferDeviceIndex<baked::Index> indices;
        GpuBufferDeviceSrv<SceneInstance> instances;
    } scene;

    struct {
        GpuPipeline threshold_pipeline;
        GpuPipeline downsample_pipeline;
        GpuPipeline upsample_pipeline;
        GpuTextureSrvUav downsample;
        GpuTextureSrvUav upsample;
    } compute;

    struct {
        RenderTarget render_target;
        RenderTargetView render_target_view;
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
