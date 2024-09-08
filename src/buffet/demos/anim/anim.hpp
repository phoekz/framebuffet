#pragma once

#include "../common.hpp"
#include "anim.hlsli"

namespace fb::demos::anim {

inline constexpr std::string_view NAME = "Anim"sv;
inline constexpr KcnColorAttachmentDescs COLOR_ATTACHMENTS = {KcnColorAttachmentDesc {
    .format = DXGI_FORMAT_R8G8B8A8_UNORM,
    .clear_color = {0.0f, 0.0f, 0.0f, 1.0f},
}};
inline constexpr KcnDepthStencilAttachmentDesc DEPTH_STENCIL_ATTACHMENT = {
    .format = DXGI_FORMAT_D32_FLOAT,
    .clear_depth = 1.0f,
    .clear_stencil = 0,
};
inline constexpr uint SAMPLE_COUNT = 4;

struct Parameters {
    float time_scale = 1.0f;
    float camera_distance = 3.5f;
    float camera_height_offset = 0.5f;
    float camera_fov = rad_from_deg(50.0f);
    float camera_latitude = rad_from_deg(30.0f);
    float camera_longitude = rad_from_deg(120.0f);
    float camera_rotation_speed = 0.0f;
    float2 camera_clip_planes = float2(0.1f, 300.0f);
    std::array<float4, 2> colors = {
        float4(0.173f, 0.173f, 0.173f, 1.000f),
        float4(0.905f, 0.167f, 0.167f, 1.000f),
    };
    std::array<float3, 2> positions = {
        float3(-0.75f, 0.0f, 0.0f),
        float3(0.75f, 0.0f, 0.0f),
    };
    float texture_scroll_speed = 2.5f;
};

struct OwnedAnimationMesh {
    float4x4 transform;
    uint node_count;
    uint joint_count;
    float duration;
    std::vector<baked::SkinningVertex> skinning_vertices;
    std::vector<baked::Index> indices;
    std::vector<baked::Submesh> submeshes;
    std::vector<uint> joint_nodes;
    std::vector<float4x4> joint_inverse_binds;
    std::vector<uint> node_parents;
    std::vector<baked::AnimationChannel> node_channels;
    std::vector<float> node_channels_times_t;
    std::vector<float> node_channels_times_r;
    std::vector<float> node_channels_times_s;
    std::vector<float3> node_channels_values_t;
    std::vector<float_quat> node_channels_values_r;
    std::vector<float3> node_channels_values_s;
};

struct Model {
    KcnMultibuffer<GpuBufferHostCbv<Constants>, FRAME_COUNT> constants;
    KcnMultibuffer<GpuBufferHostSrv<float4x4>, FRAME_COUNT> skinning_matrices;
    GpuBufferDeviceSrv<baked::SkinningVertex> vertices;
    GpuBufferDeviceIndex<baked::Index> indices;
    float animation_time = 0.0f;
    float animation_duration = 0.0f;
    std::vector<float4x4> animation_global_transforms;
    OwnedAnimationMesh animation_mesh;
};

struct Anim {
    GpuPipeline pipeline;
    Model female;
    Model male;
};

struct Ground {
    KcnMultibuffer<GpuBufferHostCbv<Constants>, FRAME_COUNT> constants;
    GpuBufferDeviceSrv<baked::Vertex> vertices;
    GpuBufferDeviceIndex<baked::Index> indices;
    GpuTextureSrv texture;
    float texture_scroll = 0.0f;
    GpuPipeline pipeline;
};

struct Demo {
    Parameters parameters;
    KcnRenderTarget render_target;
    KcnRenderTargetView render_target_view;
    KcnDebugDraw debug_draw;

    Ground ground;
    Anim anim;
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

} // namespace fb::demos::anim
