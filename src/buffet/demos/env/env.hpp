#pragma once

#include "../demos.hpp"
#include "env.hlsli"

namespace fb::demos::env {

struct Parameters {
    uint32_t tonemap = (uint32_t)(true);
    float camera_fov = rad_from_deg(70.0f);
    float camera_distance = 1.25f;
    float camera_longitude = rad_from_deg(45.0f);
    float camera_latitude = rad_from_deg(0.0f);
    float camera_rotation_speed = 0.5f;
};

class EnvDemo {
    FB_NO_COPY_MOVE(EnvDemo);

public:
    static constexpr std::string_view NAME = "Env"sv;
    static constexpr Float4 CLEAR_COLOR = {0.3f, 0.6f, 0.0f, 1.0f};

    EnvDemo() = default;

    auto create(GpuDevice& device, const baked::Assets& assets, const baked::Shaders& shaders)
        -> void;
    auto gui(const GuiDesc& desc) -> void;
    auto update(const UpdateDesc& desc) -> void;
    auto render(GpuDevice& device, GpuCommandList& cmd) -> void;
    auto rt() -> RenderTargets& { return _render_targets; }

    template<Archive A>
    auto archive(A& arc) -> void {
        arc& _parameters;
    }

private:
    Parameters _parameters;
    RenderTargets _render_targets;
    DebugDraw _debug_draw;

    struct {
        GpuPipeline pipeline;
        GpuBufferHostCbv<BackgroundConstants> constants;
        GpuBufferHostSrv<baked::Vertex> vertices;
        GpuBufferHostIndex<baked::Index> indices;
        GpuTextureSrvCube texture;
    } _background;

    struct {
        GpuBufferHostCbv<ComputeConstants> constants;

        Uint2 rect_texture_size;
        GpuTextureSrv rect_texture;

        Uint2 cube_texture_size;
        GpuTextureSrvUavCube cube_texture;

        Uint2 lut_texture_size;
        GpuTextureSrvUav lut_texture;

        GpuPipeline cfr_pipeline;
        GpuPipeline lut_pipeline;

        bool completed = false;
    } _compute;

    struct {
        Float2 rect_offset;
        Float2 rect_scale;

        std::array<Float2, 6> cube_offsets;
        Float2 cube_scale;

        Float2 lut_offset;
        Float2 lut_scale;

        GpuBufferHostCbv<ScreenConstants> constants;
        GpuBufferHostSrv<ScreenVertex> vertices;
        GpuBufferHostIndex<baked::Index> indices;
        GpuPipeline pipeline;
    } _screen;
};

} // namespace fb::demos::env
