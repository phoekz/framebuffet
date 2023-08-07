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
    auto update(const demo::UpdateDesc& desc) -> void;
    auto render(GpuDevice& device) -> void;

    auto rt_color() const -> const fb::ComPtr<ID3D12Resource>& { return _render_targets.color(); }

  private:
    GpuDescriptors _descriptors;
    GpuSamplers _samplers;
    GpuRenderTargets _render_targets;
    GpuDebugDraw _debug_draw;

    ComPtr<ID3D12PipelineState> _pipeline_state;
    GpuBufferHostCbv<Constants> _constant_buffer;
    GpuBufferHostSrv<Vertex> _vertex_buffer;
    GpuBufferHostIndex<Index> _index_buffer;
    ComPtr<ID3D12Resource> _env_texture;

    GpuDescriptorHandle _constant_buffer_descriptor;
    GpuDescriptorHandle _vertex_buffer_descriptor;
    GpuDescriptorHandle _env_texture_descriptor;
};

}  // namespace fb::env
