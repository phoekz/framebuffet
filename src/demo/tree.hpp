#pragma once

#include "../utils.hpp"
#include "../maths.hpp"
#include "../dx12.hpp"
#include "../buffers.hpp"
#include "../gltf.hpp"
#include "../descriptors.hpp"
#include "../samplers.hpp"
#include "../root_signature.hpp"

namespace fb::tree {

constexpr uint32_t SHADOW_MAP_SIZE = 1024;

struct ShadowConstants {
    Matrix transform;
    float pad[48];
};

struct MainConstants {
    Matrix transform;
    Matrix light_transform;
    Vector3 light_direction = {0.577f, 0.577f, 0.577f};  // normalized(1,1,1)
    float ambient_light = 0.25f;
    float pad[28];
};

struct UpdateParams {
    float aspect_ratio;
    float elapsed_time;
};

struct Demo {
    static constexpr const char* NAME = "Tree";

    Demo(Dx& dx);
    void update(const UpdateParams& params);
    void render(Dx& dx);

    GpuRootSignature root_signature;
    GpuDescriptors descriptors;
    GpuSamplers samplers;

    struct Scene {
        struct Model {
            GpuBuffer<GltfVertex> vertex_buffer;
            GpuDescriptorHandle vertex_buffer_descriptor;
            GpuBuffer<GltfIndex> index_buffer;
            ComPtr<ID3D12Resource> texture;
            GpuDescriptorHandle texture_descriptor;
        };
        Model tree;
        Model plane;
    } scene;

    struct ShadowPass {
        static constexpr const char* NAME = "Shadow";

        ComPtr<ID3D12PipelineState> pipeline_state;

        GpuBuffer<ShadowConstants> constants;
        GpuDescriptorHandle constants_descriptor;

        ComPtr<ID3D12Resource> depth;
        GpuDescriptorHandle depth_dsv_descriptor;
        GpuDescriptorHandle depth_srv_descriptor;
    } shadow_pass;

    struct MainPass {
        static constexpr const char* NAME = "Main";

        ComPtr<ID3D12PipelineState> pipeline_state;

        GpuBuffer<MainConstants> constants;
        GpuDescriptorHandle constants_descriptor;
    } main_pass;

    struct Target {
        ComPtr<ID3D12Resource> color;
        GpuDescriptorHandle color_descriptor;

        ComPtr<ID3D12Resource> depth;
        GpuDescriptorHandle depth_descriptor;
    } target;
};

}  // namespace fb::tree
