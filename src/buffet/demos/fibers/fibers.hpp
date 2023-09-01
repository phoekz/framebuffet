#pragma once

#include "../common.hpp"
#include "fibers.hlsli"

namespace fb::demos::fibers {

inline constexpr std::string_view NAME = "Fibers"sv;
inline constexpr float4 CLEAR_COLOR = {0.0f, 0.0f, 0.0f, 1.0f};
inline constexpr DXGI_FORMAT COLOR_FORMAT = DXGI_FORMAT_R16G16B16A16_FLOAT;
inline constexpr uint SAMPLE_COUNT = 1;

enum class Heatmap : uint {
    Magma,
    Viridis,
};

struct Parameters {
    float camera_distance = 2.2f;
    float camera_fov = rad_from_deg(45.0f);
    float camera_latitude = rad_from_deg(0.0f);
    float camera_longitude = rad_from_deg(90.0f);
    float2 camera_clip_planes = float2(0.1f, 100.0f);
    bool show_light_bounds = true;

    float light_speed = 0.25f;
    int light_intensity_pow2 = 12;
    float light_range = 0.25f;

    Heatmap heatmap = Heatmap::Magma;
    float heatmap_opacity = 0.5f;
};

struct Mesh {
    GpuBufferHostSrv<baked::Vertex> vertices;
    GpuBufferHostIndex<baked::Index> indices;
};

struct Demo {
    Parameters parameters;
    RenderTargets render_targets;
    DebugDraw debug_draw;
    GpuPipeline sim_pipeline;
    GpuPipeline reset_pipeline;
    GpuPipeline cull_pipeline;
    GpuPipeline light_pipeline;
    GpuPipeline plane_pipeline;
    GpuPipeline debug_pipeline;
    GpuBufferHostCbv<Constants> constants;
    Mesh light_mesh;
    Mesh plane_mesh;
    GpuBufferDeviceSrvUav<Light> lights;
    GpuTextureSrv magma_texture;
    GpuTextureSrv viridis_texture;
    GpuTextureSrvUav light_counts_texture;
    GpuTextureSrvUav light_offsets_texture;
    GpuBufferDeviceSrvUav<uint> light_indices;
    GpuBufferDeviceSrvUav<uint> light_indices_count;
    uint sim_dispatch_count_x = 0;
    uint cull_dispatch_count_x = 0;
    uint cull_dispatch_count_y = 0;
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

} // namespace fb::demos::fibers
