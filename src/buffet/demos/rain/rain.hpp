#pragma once

#include "../demos.hpp"

namespace fb::demos::rain {

inline constexpr uint32_t PARTICLE_COUNT = 4 * 1024;
inline constexpr uint32_t DISPATCH_SIZE = 128;
inline constexpr uint32_t DISPATCH_COUNT = PARTICLE_COUNT / DISPATCH_SIZE;

struct Constants {
    Float4x4 transform;
    Float4x4 particle_transform;
    float delta_time = 0.0f;
    float speed = 0.5f;
    float pad[30] = {};
};

struct Particle {
    Float3 position;
};
static_assert(sizeof(Particle) == 12);

struct Vertex {
    Float3 position;
    Float2 texcoord;
};

class RainDemo {
  public:
    static constexpr std::string_view NAME = "Rain"sv;
    static constexpr Float4 CLEAR_COLOR = {0.2f, 0.2f, 0.2f, 1.0f};

    RainDemo(GpuDevice& device, const baked::Assets& assets, const baked::Shaders& shaders);
    auto gui(const GuiDesc& desc) -> void;
    void update(const UpdateDesc& desc);
    void render(GpuDevice& device, GpuCommandList& cmd);

    auto rt_color() const -> const GpuTexture2dSrvRtv& { return _render_targets.color(); }

  private:
    RenderTargets _render_targets;
    DebugDraw _debug_draw;
    GpuBufferDeviceSrvUav<Particle> _particles;

    GpuBufferHostCbv<Constants> _constants;
    GpuPipeline _compute_pipeline;
    GpuPipeline _draw_pipeline;
    GpuBufferHostSrv<Vertex> _draw_vertices;
    GpuBufferHostIndex<uint16_t> _draw_indices;
};

}  // namespace fb::demos::rain
