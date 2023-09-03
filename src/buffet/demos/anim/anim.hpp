#pragma once

#include "../common.hpp"
#include "anim.hlsli"

namespace fb::demos::anim {

inline constexpr std::string_view NAME = "Anim"sv;
inline constexpr float4 CLEAR_COLOR = {0.6f, 0.2f, 0.6f, 1.0f};
inline constexpr DXGI_FORMAT COLOR_FORMAT = DXGI_FORMAT_R8G8B8A8_UNORM;
inline constexpr uint SAMPLE_COUNT = 4;

struct Parameters {
    float camera_distance = 4.0f;
    float camera_fov = rad_from_deg(45.0f);
    float camera_latitude = rad_from_deg(30.0f);
    float camera_longitude = rad_from_deg(0.0f);
    float camera_rotation_speed = 0.5f;
    float2 camera_clip_planes = float2(0.1f, 300.0f);
};

struct Demo {
    Parameters parameters;
    RenderTargets render_targets;
    DebugDraw debug_draw;
    GpuPipeline pipeline;
    Multibuffer<GpuBufferHostCbv<Constants>, FRAME_COUNT> constants;
    GpuBufferHostSrv<baked::SkinningVertex> vertices;
    GpuBufferHostIndex<baked::Index> indices;
    GpuBufferHostSrv<float4x4> joint_inverse_bind_buffer;
    GpuBufferHostSrv<float4x4> joint_global_transform_buffer;
    GpuTextureSrv texture;
    float animation_time = 0.0f;
    float animation_duration = 0.0f;
    std::vector<float4x4> node_global_transforms;
    baked::AnimationMesh animation_mesh;
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

} // namespace fb::demos::anim
