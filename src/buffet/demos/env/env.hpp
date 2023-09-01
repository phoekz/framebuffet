#pragma once

#include "../demos.hpp"
#include "env.hlsli"

namespace fb::demos::env {

struct Parameters {
    float model_metallic = 0.0f;
    float model_roughness = 0.0f;
    uint tonemap = (uint)(true);
    float exposure = 0.0f;
    float background_roughness = 0.0f;
    float camera_fov = rad_from_deg(90.0f);
    float camera_distance = 3.0f;
    float camera_longitude = rad_from_deg(45.0f);
    float camera_latitude = rad_from_deg(0.0f);
    float camera_rotation_speed = 0.25f;
};

class EnvDemo {
    FB_NO_COPY_MOVE(EnvDemo);

public:
    static constexpr std::string_view NAME = "Env"sv;
    static constexpr float4 CLEAR_COLOR = {0.3f, 0.6f, 0.0f, 1.0f};

    EnvDemo() = default;

    auto create(GpuDevice& device, const Baked& baked) -> void;
    auto gui(const GuiDesc& desc) -> void;
    auto update(const UpdateDesc& desc) -> void;
    auto render(GpuDevice& device, GpuCommandList& cmd) -> void;
    auto rt() -> graphics::render_targets::RenderTargets& { return _render_targets; }

    template<Archive A>
    auto archive(A& arc) -> void {
        arc& _parameters;
    }

private:
    Parameters _parameters;
    graphics::render_targets::RenderTargets _render_targets;
    graphics::debug_draw::DebugDraw _debug_draw;

    struct {
        GpuTextureSrv lut;
        GpuTextureSrvCube irr;
        GpuTextureSrvCube rad;
    } _pbr;

    struct {
        GpuPipeline pipeline;
        GpuBufferHostCbv<BackgroundConstants> constants;
        GpuBufferHostSrv<baked::Vertex> vertices;
        GpuBufferHostIndex<baked::Index> indices;
        GpuTextureSrvCube texture;
    } _background;

    struct {
        GpuPipeline pipeline;
        GpuBufferHostCbv<ModelConstants> constants;
        GpuBufferHostSrv<baked::Vertex> vertices;
        GpuBufferHostIndex<baked::Index> indices;
    } _model;
};

} // namespace fb::demos::env
