#pragma once

#include "../utils.hpp"
#include "../maths.hpp"
#include "../dx12.hpp"
#include "../buffers.hpp"
#include "../gltf.hpp"
#include "../descriptors.hpp"
#include "../samplers.hpp"
#include "../root_signature.hpp"
#include "../render_targets.hpp"
#include <pch.hpp>

namespace fb::cube {

struct Constants {
    Matrix transform;
    float pad[48];
};

struct UpdateParams {
    float aspect_ratio;
    float elapsed_time;
};

struct Demo {
    static constexpr std::string_view NAME = "Cube"sv;
    static constexpr Vector4 CLEAR_COLOR = {0.6f, 0.3f, 0.0f, 1.0f};

    Demo(Dx& dx);
    void update(const UpdateParams& params);
    void render(Dx& dx);

    GpuRootSignature root_signature;
    GpuDescriptors descriptors;
    GpuSamplers samplers;
    GpuRenderTargets render_targets;

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
