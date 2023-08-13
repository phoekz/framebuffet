#pragma once

#include "demo.hpp"

#include <shaders/fibers.hlsli>

namespace fb::fibers {

constexpr uint32_t DISPATCH_COUNT = (LIGHT_COUNT + (DISPATCH_SIZE - 1)) / DISPATCH_SIZE;

struct Constants {
    Float4x4 transform;
    float delta_time = 0.0f;
    float light_speed = 0.5f;
    float light_range = 0.1f;
    float light_intensity = 0.0f;
    float pad[44] = {};
};

struct Vertex {
    Float3 position;
    Float3 normal;
    Float2 texcoord;
};

using Index = uint16_t;

struct Mesh {
    GpuBufferHostSrv<Vertex> vertex_buffer;
    GpuBufferHostIndex<Index> index_buffer;
};

struct Light {
    Float3 position;
    Float3 color;
    float speed_variation;
};

class Demo {
  public:
    static constexpr std::string_view NAME = "Fibers"sv;
    static constexpr Float4 CLEAR_COLOR = {0.05f, 0.05f, 0.05f, 1.0f};

    Demo(GpuDevice& device);
    auto gui(const gui::Desc& desc) -> void;
    auto update(const demo::UpdateDesc& desc) -> void;
    auto render(GpuDevice& device, GpuCommandList& cmd) -> void;

    auto rt_color() const -> const GpuTexture2dSrvRtv& { return _render_targets.color(); }

  private:
    GpuRenderTargets _render_targets;
    GpuDebugDraw _debug_draw;
    GpuPipeline _sim_pipeline;
    GpuPipeline _light_pipeline;
    GpuPipeline _plane_pipeline;
    GpuBufferHostCbv<Constants> _constant_buffer;
    Mesh _light_mesh;
    Mesh _plane_mesh;
    GpuBufferDeviceSrvUav<Light> _light_buffer;
};

}  // namespace fb::fibers
