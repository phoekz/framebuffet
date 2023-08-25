#pragma once

#include "../demos.hpp"
#include "env.hlsli"

namespace fb::demos::env {

struct Parameters {
    uint32_t tonemap = (uint32_t)true;
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

    GpuPipeline _pipeline;
    GpuBufferHostCbv<Constants> _constants;
    GpuBufferHostSrv<baked::Vertex> _vertices;
    GpuBufferHostIndex<baked::Index> _indices;
    GpuTextureSrvCube _env_texture;
};

} // namespace fb::demos::env
