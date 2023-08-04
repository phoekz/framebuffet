#pragma once

#include "demo.hpp"

namespace fb::cube {

struct Constants {
    Matrix transform;
    float pad[48];
};

struct UpdateParams {
    float aspect_ratio;
    float elapsed_time;
    uint32_t frame_index;
};

struct Demo {
    static constexpr std::string_view NAME = "Cube"sv;
    static constexpr Vector4 CLEAR_COLOR = {0.6f, 0.3f, 0.0f, 1.0f};

    Demo(GpuDevice& device);
    void update(const demo::UpdateDesc& desc);
    void render(GpuDevice& device);

    GpuRootSignature root_signature;
    GpuDescriptors descriptors;
    GpuSamplers samplers;
    GpuRenderTargets render_targets;
    GpuDebugDraw debug_draw;

    ComPtr<ID3D12PipelineState> pipeline_state;

    GpuBufferHostCbv<Constants> constant_buffer;
    GpuDescriptorHandle constant_buffer_descriptor;

    GpuBufferHostSrv<GltfVertex> vertex_buffer;
    GpuDescriptorHandle vertex_buffer_descriptor;

    GpuBufferHostIndex<GltfIndex> index_buffer;

    ComPtr<ID3D12Resource> texture;
    GpuDescriptorHandle texture_descriptor;
};

}  // namespace fb::cube
