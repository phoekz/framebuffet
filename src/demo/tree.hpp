#pragma once

#include "demo.hpp"

namespace fb::tree {

constexpr uint32_t SHADOW_MAP_SIZE = 1024;

struct ShadowConstants {
    Matrix transform;
    float pad[48];
};

struct MainConstants {
    Matrix transform;
    Matrix light_transform;
    Vector3 light_direction;
    float ambient_light = 0.25f;
    float pad[28];
};

struct Demo {
    static constexpr std::string_view NAME = "Tree"sv;
    static constexpr Vector4 CLEAR_COLOR = {0.32549f, 0.51373f, 0.56078f, 1.0f};

    Demo(GpuDevice& device);
    void update(const demo::UpdateDesc& desc);
    void render(GpuDevice& device);

    GpuRootSignature root_signature;
    GpuDescriptors descriptors;
    GpuSamplers samplers;
    GpuRenderTargets render_targets;
    GpuDebugDraw debug_draw;

    struct Scene {
        struct Model {
            GpuBufferHostSrv<GltfVertex> vertex_buffer;
            GpuDescriptorHandle vertex_buffer_descriptor;
            GpuBufferHostIndex<GltfIndex> index_buffer;
            ComPtr<ID3D12Resource> texture;
            GpuDescriptorHandle texture_descriptor;
        };
        Model tree;
        Model plane;
    } scene;

    struct ShadowPass {
        static constexpr std::string_view NAME = "Shadow"sv;

        ComPtr<ID3D12PipelineState> pipeline_state;

        GpuBufferHostCbv<ShadowConstants> constants;
        GpuDescriptorHandle constants_descriptor;

        ComPtr<ID3D12Resource> depth;
        GpuDescriptorHandle depth_dsv_descriptor;
        GpuDescriptorHandle depth_srv_descriptor;
    } shadow_pass;

    struct MainPass {
        static constexpr std::string_view NAME = "Main"sv;

        ComPtr<ID3D12PipelineState> pipeline_state;

        GpuBufferHostCbv<MainConstants> constants;
        GpuDescriptorHandle constants_descriptor;
    } main_pass;
};

}  // namespace fb::tree
