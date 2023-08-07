#pragma once

#include "demo.hpp"

namespace fb::rain {

constexpr uint32_t PARTICLE_COUNT = 4 * 1024;
constexpr uint32_t DISPATCH_SIZE = 128;
constexpr uint32_t DISPATCH_COUNT = PARTICLE_COUNT / DISPATCH_SIZE;

struct ComputeConstants {
    float delta_time = 0.0f;
    float speed = 0.5f;
    float pad[62] = {};
};

struct DrawConstants {
    Matrix transform;
    Matrix particle_transform;
    float pad[32] = {};
};

struct Particle {
    Vector3 position;
};
static_assert(sizeof(Particle) == 12);

struct Vertex {
    Vector3 position;
    Vector2 texcoord;
};

class Demo {
  public:
    static constexpr std::string_view NAME = "Rain"sv;
    static constexpr Vector4 CLEAR_COLOR = {0.2f, 0.2f, 0.2f, 1.0f};

    Demo(GpuDevice& device);
    void update(const demo::UpdateDesc& desc);
    void render(GpuDevice& device);

    auto rt_color() const -> const GpuTexture2dSrvRtv& { return _render_targets.color(); }

  private:
    GpuRenderTargets _render_targets;
    GpuDebugDraw _debug_draw;
    GpuBufferDeviceSrvUav<Particle> _particle_buffer;

    struct Compute {
        static constexpr std::string_view NAME = "Compute"sv;

        ComPtr<ID3D12PipelineState> pipeline_state;
        GpuBufferHostCbv<ComputeConstants> constant_buffer;
    } _compute;

    struct Draw {
        static constexpr std::string_view NAME = "Draw"sv;

        ComPtr<ID3D12PipelineState> pipeline_state;
        GpuBufferHostCbv<DrawConstants> constant_buffer;
        GpuBufferHostSrv<Vertex> vertex_buffer;
        GpuBufferHostIndex<uint16_t> index_buffer;
    } _draw;
};

}  // namespace fb::rain
