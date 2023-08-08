#pragma once

#include "demo.hpp"

namespace fb::env {

enum class CubeFace : uint32_t {
    PosX,
    NegX,
    PosY,
    NegY,
    PosZ,
    NegZ,
    Count,
};

struct Constants {
    Matrix transform;
    float pad[48] = {};
};

struct Vertex {
    Vector3 position;
    Vector3 normal;
    Vector2 texcoord;
};

using Index = uint16_t;

class Demo {
  public:
    static constexpr std::string_view NAME = "Env"sv;
    static constexpr Vector4 CLEAR_COLOR = {0.3f, 0.6f, 0.0f, 1.0f};

    Demo(GpuDevice& device);
    auto gui(const gui::Desc& desc) -> void;
    auto update(const demo::UpdateDesc& desc) -> void;
    auto render(GpuDevice& device, GpuCommandList& cmd) -> void;

    auto rt_color() const -> const GpuTexture2dSrvRtv& { return _render_targets.color(); }

  private:
    GpuRenderTargets _render_targets;
    GpuDebugDraw _debug_draw;

    GpuPipeline _pipeline;
    GpuBufferHostCbv<Constants> _constant_buffer;
    GpuBufferHostSrv<Vertex> _vertex_buffer;
    GpuBufferHostIndex<Index> _index_buffer;
    GpuTexture2dSrvCube _env_texture;
};

}  // namespace fb::env
