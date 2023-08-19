#pragma once

#include "../demos.hpp"
#include "fibers.hlsli"

namespace fb::demos::fibers {

enum class Heatmap : uint32_t {
    Magma,
    Viridis,
};

struct Parameters {
    float camera_distance = 4.0f;
    float camera_fov = rad_from_deg(45.0f);
    float camera_latitude = rad_from_deg(0.0f);
    float camera_longitude = rad_from_deg(90.0f);
    Float2 camera_clip_planes = Float2(0.1f, 100.0f);
    bool show_light_bounds = true;

    float light_speed = 0.5f;
    int light_intensity_pow2 = 12;
    float light_range = 0.1f;

    Heatmap heatmap = Heatmap::Magma;
    float heatmap_opacity = 0.5f;
};

inline constexpr uint32_t SIM_DISPATCH_COUNT =
    (LIGHT_COUNT + (SIM_DISPATCH_SIZE - 1)) / SIM_DISPATCH_SIZE;
inline constexpr uint32_t CULL_DISPATCH_COUNT_X =
    (1280 + (CULL_DISPATCH_SIZE - 1)) / CULL_DISPATCH_SIZE;
inline constexpr uint32_t CULL_DISPATCH_COUNT_Y =
    (800 + (CULL_DISPATCH_SIZE - 1)) / CULL_DISPATCH_SIZE;

struct Mesh {
    GpuBufferHostSrv<baked::Vertex> vertices;
    GpuBufferHostIndex<baked::Index> indices;
};

class FibersDemo {
public:
    static constexpr std::string_view NAME = "Fibers"sv;
    static constexpr Float4 CLEAR_COLOR = {0.05f, 0.05f, 0.05f, 1.0f};

    FibersDemo(GpuDevice& device, const baked::Assets& assets, const baked::Shaders& shaders);
    auto gui(const GuiDesc& desc) -> void;
    auto update(const UpdateDesc& desc) -> void;
    auto render(GpuDevice& device, GpuCommandList& cmd) -> void;

    auto rt_color() const -> const GpuTextureSrvRtv& { return _render_targets.color(); }

    template<Archive A>
    auto archive(A& arc) -> void {
        arc& _parameters;
    }

private:
    Parameters _parameters;
    RenderTargets _render_targets;
    DebugDraw _debug_draw;
    GpuPipeline _sim_pipeline;
    GpuPipeline _reset_pipeline;
    GpuPipeline _cull_pipeline;
    GpuPipeline _light_pipeline;
    GpuPipeline _plane_pipeline;
    GpuPipeline _debug_pipeline;
    GpuBufferHostCbv<Constants> _constants;
    Mesh _light_mesh;
    Mesh _plane_mesh;
    GpuBufferDeviceSrvUav<Light> _lights;
    GpuTextureSrv _magma_texture;
    GpuTextureSrv _viridis_texture;
    GpuTextureSrvUav _light_counts_texture;
    GpuTextureSrvUav _light_offsets_texture;
    GpuBufferDeviceSrvUav<uint32_t> _light_indices;
    GpuBufferDeviceSrvUav<uint32_t> _light_indices_count;
};

} // namespace fb::demos::fibers
