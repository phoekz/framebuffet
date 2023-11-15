#pragma once

#include "../common.hpp"
#include "env.hlsli"

namespace fb::demos::env {

inline constexpr std::string_view NAME = "Env"sv;
inline constexpr DXGI_FORMAT COLOR_FORMAT = DXGI_FORMAT_R16G16B16A16_FLOAT;
inline constexpr float4 COLOR_CLEAR_VALUE = {0.3f, 0.6f, 0.0f, 1.0f};
inline constexpr DXGI_FORMAT DEPTH_FORMAT = DXGI_FORMAT_D32_FLOAT;
inline constexpr float DEPTH_CLEAR_VALUE = 1.0f;
inline constexpr uint SAMPLE_COUNT = 4;

struct Parameters {
    float model_metallic = 0.0f;
    float model_roughness = 0.15f;
    uint tonemap = (uint)(true);
    float exposure = 0.0f;
    float background_roughness = 0.15f;
    float camera_fov = rad_from_deg(90.0f);
    float camera_distance = 3.0f;
    float camera_longitude = rad_from_deg(45.0f);
    float camera_latitude = rad_from_deg(0.0f);
    float camera_rotation_speed = 0.25f;
};

struct Demo {
    Parameters parameters;
    RenderTargets render_targets;
    DebugDraw debug_draw;

    struct {
        GpuTextureSrv lut;
        GpuTextureSrvCube irr;
        GpuTextureSrvCube rad;
    } pbr;

    struct {
        GpuPipeline pipeline;
        Multibuffer<GpuBufferHostCbv<BackgroundConstants>, FRAME_COUNT> constants;
        GpuBufferHostSrv<baked::Vertex> vertices;
        GpuBufferHostIndex<baked::Index> indices;
        GpuTextureSrvCube texture;
    } background;

    struct {
        GpuPipeline pipeline;
        Multibuffer<GpuBufferHostCbv<ModelConstants>, FRAME_COUNT> constants;
        GpuBufferHostSrv<baked::Vertex> vertices;
        GpuBufferHostIndex<baked::Index> indices;
    } model;
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
    arc& demo.parameters;
}

} // namespace fb::demos::env
