#pragma once

#include "../demos.hpp"

namespace fb::demos::env {

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
    Float4x4 transform;
    float pad[48] = {};
};

class EnvDemo {
  public:
    static constexpr std::string_view NAME = "Env"sv;
    static constexpr Float4 CLEAR_COLOR = {0.3f, 0.6f, 0.0f, 1.0f};

    EnvDemo(GpuDevice& device, const baked::Assets& assets, const baked::Shaders& shaders);
    auto gui(const GuiDesc& desc) -> void;
    auto update(const UpdateDesc& desc) -> void;
    auto render(GpuDevice& device, GpuCommandList& cmd) -> void;

    auto rt_color() const -> const GpuTextureSrvRtv& { return _render_targets.color(); }

  private:
    RenderTargets _render_targets;
    DebugDraw _debug_draw;

    GpuPipeline _pipeline;
    GpuBufferHostCbv<Constants> _constants;
    GpuBufferHostSrv<baked::Vertex> _vertices;
    GpuBufferHostIndex<baked::Index> _indices;
    GpuTextureSrvCube _env_texture;
};

} // namespace fb::demos::env
