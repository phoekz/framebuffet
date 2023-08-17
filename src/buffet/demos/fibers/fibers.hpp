#pragma once

#include "../demos.hpp"

#include "fibers.hlsli"

namespace fb::demos::fibers {

inline constexpr uint32_t SIM_DISPATCH_COUNT =
    (LIGHT_COUNT + (SIM_DISPATCH_SIZE - 1)) / SIM_DISPATCH_SIZE;
inline constexpr uint32_t CULL_DISPATCH_COUNT_X =
    (1280 + (CULL_DISPATCH_SIZE - 1)) / CULL_DISPATCH_SIZE;
inline constexpr uint32_t CULL_DISPATCH_COUNT_Y =
    (800 + (CULL_DISPATCH_SIZE - 1)) / CULL_DISPATCH_SIZE;

struct Constants {
    Float4x4 clip_from_world;
    Float4x4 view_from_clip;
    Float4x4 view_from_world;
    Float2 window_size;
    float delta_time = 0.0f;
    float light_speed = 0.5f;
    float light_range = 0.1f;
    float light_intensity = 0.0f;
    float debug_opacity = 0.5f;
    float pad[9] = {};
};

struct Mesh {
    GpuBufferHostSrv<baked::Vertex> vertices;
    GpuBufferHostIndex<baked::Index> indices;
};

struct Light {
    Float3 position;
    Float3 color;
    float speed_variation;
};

class FibersDemo {
  public:
    static constexpr std::string_view NAME = "Fibers"sv;
    static constexpr Float4 CLEAR_COLOR = {0.05f, 0.05f, 0.05f, 1.0f};

    FibersDemo(GpuDevice& device, const baked::Assets& assets, const baked::Shaders& shaders);
    auto gui(const GuiDesc& desc) -> void;
    auto update(const UpdateDesc& desc) -> void;
    auto render(GpuDevice& device, GpuCommandList& cmd) -> void;

    auto rt_color() const -> const GpuTexture2dSrvRtv& { return _render_targets.color(); }

  private:
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
    GpuTexture2dSrv _magma_texture;
    GpuTexture2dSrv _viridis_texture;
    GpuTexture2dSrvUav _light_counts_texture;
    GpuTexture2dSrvUav _light_offsets_texture;
    GpuBufferDeviceSrvUav<uint32_t> _light_indices;
    GpuBufferDeviceSrvUav<uint32_t> _light_indices_count;
};

}  // namespace fb::demos::fibers
