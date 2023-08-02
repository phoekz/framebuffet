#pragma once

#include "../utils.hpp"
#include "../maths.hpp"
#include "../dx12.hpp"
#include "../buffers.hpp"
#include "../gltf.hpp"
#include "../descriptors.hpp"
#include "../samplers.hpp"
#include "../root_signature.hpp"
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
    static constexpr const char* NAME = "Cube";

    Demo(Dx& dx);
    void update(const UpdateParams& params);
    void render(Dx& dx);

    GpuRootSignature root_signature;
    GpuDescriptors descriptors;
    GpuSamplers samplers;

    ComPtr<ID3D12PipelineState> pipeline_state;

    GpuBuffer<Constants> constant_buffer;
    GpuDescriptorHandle constant_buffer_descriptor;

    GpuBuffer<GltfVertex> vertex_buffer;
    GpuBuffer<GltfIndex> index_buffer;

    ComPtr<ID3D12Resource> texture;
    GpuDescriptorHandle texture_descriptor;

    ComPtr<ID3D12Resource> color_target;
    GpuDescriptorHandle color_target_descriptor;

    ComPtr<ID3D12Resource> depth_target;
    GpuDescriptorHandle depth_target_descriptor;
};

}  // namespace fb::cube
