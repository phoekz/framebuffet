#pragma once

#include "demo.hpp"

namespace fb::cube {

struct Constants {
    Matrix transform;
    float pad[48] = {};
};

class Demo {
  public:
    static constexpr std::string_view NAME = "Cube"sv;
    static constexpr Vector4 CLEAR_COLOR = {0.6f, 0.3f, 0.0f, 1.0f};

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
    GpuBufferHostSrv<GltfVertex> _vertex_buffer;
    GpuBufferHostIndex<GltfIndex> _index_buffer;
    GpuTexture2dSrv _texture;
};

}  // namespace fb::cube
