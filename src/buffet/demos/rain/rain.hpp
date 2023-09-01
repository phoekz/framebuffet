#pragma once

#include "../demos.hpp"

#include "rain.hlsli"

namespace fb::demos::rain {

inline constexpr uint PARTICLE_COUNT = 4 * 1024;
inline constexpr uint DISPATCH_COUNT = PARTICLE_COUNT / SIM_DISPATCH_SIZE;

struct Parameters {
    float speed = 0.5f;
    float camera_distance = 1.25f;
    float camera_longitude = rad_from_deg(45.0f);
    float camera_latitude = rad_from_deg(-15.0f);
    float camera_fov = rad_from_deg(45.0f);
    float camera_rotation_speed = 0.05f;
    float particle_width = 0.01f;
    float particle_height = 0.075f;
};

struct Particle {
    float3 position;
};
static_assert(sizeof(Particle) == 12);

struct Vertex {
    float3 position;
    float2 texcoord;
};

class RainDemo {
    FB_NO_COPY_MOVE(RainDemo);

public:
    static constexpr std::string_view NAME = "Rain"sv;
    static constexpr float4 CLEAR_COLOR = {0.025f, 0.025f, 0.025f, 1.0f};

    RainDemo() = default;

    auto create(GpuDevice& device, const Baked& baked) -> void;
    auto gui(const GuiDesc& desc) -> void;
    void update(const UpdateDesc& desc);
    void render(GpuDevice& device, GpuCommandList& cmd);
    auto rt() -> graphics::render_targets::RenderTargets& { return _render_targets; }

    template<Archive A>
    auto archive(A& arc) -> void {
        arc& _parameters;
    }

private:
    Parameters _parameters;
    graphics::render_targets::RenderTargets _render_targets;
    graphics::debug_draw::DebugDraw _debug_draw;
    GpuBufferDeviceSrvUav<Particle> _particles;

    GpuBufferHostCbv<Constants> _constants;
    GpuPipeline _compute_pipeline;
    GpuPipeline _draw_pipeline;
    GpuBufferHostSrv<Vertex> _draw_vertices;
    GpuBufferHostIndex<uint16_t> _draw_indices;
};

} // namespace fb::demos::rain
