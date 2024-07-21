#pragma once

#include "../common.hpp"
#include "grass.hlsli"

namespace fb::demos::grass {

inline constexpr std::string_view NAME = "Grass"sv;
inline constexpr DXGI_FORMAT COLOR_FORMAT = DXGI_FORMAT_R8G8B8A8_UNORM;
inline constexpr float4 COLOR_CLEAR_VALUE = {1.0f / 16.0f, 1.0f / 16.0f, 1.0f / 16.0f, 1.0f};
inline constexpr DXGI_FORMAT DEPTH_FORMAT = DXGI_FORMAT_D32_FLOAT;
inline constexpr float DEPTH_CLEAR_VALUE = 1.0f;
inline constexpr uint SAMPLE_COUNT = 4;

struct Parameters {
    float camera_distance = 1.5f;
    float camera_fov = rad_from_deg(45.0f);
    float camera_latitude = rad_from_deg(30.0f);
    float camera_longitude = rad_from_deg(0.0f);
    float camera_rotation_speed = 0.05f;
    float camera_height = 0.2f;
    bool enable_atoc = true;
};

struct Demo {
    Parameters parameters;
    RenderTargets render_targets;
    DebugDraw debug_draw;
    Multibuffer<GpuBufferHostCbv<Constants>, FRAME_COUNT> constants;
    GpuBufferDeviceSrv<baked::Vertex> vertices;
    GpuBufferDeviceIndex<baked::Index> indices;
    GpuTextureSrv texture;
    baked::Material material;
    GpuPipeline pipeline_naive;
    GpuPipeline pipeline_atoc;
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

} // namespace fb::demos::grass
