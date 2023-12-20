#pragma once

#include "../common.hpp"
#include "rain.hlsli"

namespace fb::demos::rain {

inline constexpr std::string_view NAME = "Rain"sv;
inline constexpr DXGI_FORMAT COLOR_FORMAT = DXGI_FORMAT_R8G8B8A8_UNORM;
inline constexpr float4 COLOR_CLEAR_VALUE = {0.025f, 0.025f, 0.025f, 1.0f};
inline constexpr DXGI_FORMAT DEPTH_FORMAT = DXGI_FORMAT_D32_FLOAT;
inline constexpr float DEPTH_CLEAR_VALUE = 1.0f;
inline constexpr uint SAMPLE_COUNT = 1;

inline constexpr uint PARTICLE_COUNT = 4 * 1024;
inline constexpr uint DISPATCH_COUNT = PARTICLE_COUNT / DISPATCH_X;

struct Parameters {
    float speed = 0.5f;
    float camera_distance = 1.25f;
    float camera_longitude = rad_from_deg(45.0f);
    float camera_latitude = rad_from_deg(-15.0f);
    float camera_fov = rad_from_deg(60.0f);
    float camera_rotation_speed = 0.05f;
    float particle_width = 0.01f;
    float particle_height = 0.075f;
};

struct Demo {
    Parameters parameters;
    RenderTargets render_targets;
    DebugDraw debug_draw;
    GpuBufferDeviceSrvUav<Particle> particles;
    Multibuffer<GpuBufferHostCbv<Constants>, FRAME_COUNT> constants;
    GpuPipeline compute_pipeline;
    GpuPipeline draw_pipeline;
    GpuBufferHostSrv<Vertex> draw_vertices;
    GpuBufferHostIndex<uint16_t> draw_indices;
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

} // namespace fb::demos::rain
