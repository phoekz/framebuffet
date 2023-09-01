#pragma once

#include "../common.hpp"
#include "tree.hlsli"

namespace fb::demos::tree {

inline constexpr std::string_view NAME = "Tree"sv;
inline constexpr float4 CLEAR_COLOR = {0.32549f, 0.51373f, 0.56078f, 1.0f};
inline constexpr DXGI_FORMAT COLOR_FORMAT = DXGI_FORMAT_R8G8B8A8_UNORM;
inline constexpr uint SAMPLE_COUNT = 4;

inline constexpr uint SHADOW_MAP_SIZE = 1024;

struct Parameters {
    float ambient_light = 0.25f;
    float light_projection_size = 15.0f;
    float light_longitude = 0.0f;
    float light_latitude = rad_from_deg(30.0f);
    float light_distance = 15.0f;
    float shadow_near_plane = 0.1f;
    float shadow_far_plane = 100.0f;
    float camera_distance = 10.0f;
    float camera_fov = rad_from_deg(45.0f);
    float camera_latitude = rad_from_deg(30.0f);
    float camera_longitude = rad_from_deg(0.0f);
};

struct Demo {
    Parameters parameters;
    RenderTargets render_targets;
    DebugDraw debug_draw;
    GpuBufferHostCbv<Constants> constants;
    GpuBufferHostSrv<baked::Vertex> tree_vertices;
    GpuBufferHostIndex<baked::Index> tree_indices;
    GpuTextureSrv tree_texture;
    GpuBufferHostSrv<baked::Vertex> plane_vertices;
    GpuBufferHostIndex<baked::Index> plane_indices;
    GpuTextureSrv plane_texture;
    GpuPipeline shadow_pipeline;
    GpuTextureSrvDsv shadow_depth;
    GpuPipeline draw_pipeline;
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

} // namespace fb::demos::tree
