#pragma once

#include "demo.hpp"

namespace fb::rain {

constexpr uint32_t PARTICLE_COUNT = 4 * 1024;
constexpr uint32_t DISPATCH_SIZE = 128;
constexpr uint32_t DISPATCH_COUNT = PARTICLE_COUNT / DISPATCH_SIZE;

struct ComputeConstants {
    float delta_time = 0.0f;
    float speed = 0.2f;
    float pad[62] = {};
};

struct DrawConstants {
    Matrix transform;
    float pad[48] = {};
};

struct Particle {
    Vector3 position;
};
static_assert(sizeof(Particle) == 12);

struct Demo {
    static constexpr std::string_view NAME = "Rain"sv;
    static constexpr Vector4 CLEAR_COLOR = {0.0f, 0.3f, 0.6f, 1.0f};

    Demo(GpuDevice& device);
    void update(const demo::UpdateDesc& desc);
    void render(GpuDevice& device);

    GpuRootSignature root_signature;
    GpuDescriptors descriptors;
    GpuRenderTargets render_targets;
    GpuDebugDraw debug_draw;

    GpuBufferDeviceSrvUav<Particle> particle_buffer;
    GpuDescriptorHandle particle_buffer_srv_descriptor;
    GpuDescriptorHandle particle_buffer_uav_descriptor;

    struct Compute {
        static constexpr std::string_view NAME = "Compute"sv;

        ComPtr<ID3D12PipelineState> pipeline_state;

        GpuBufferHostCbv<ComputeConstants> constant_buffer;
        GpuDescriptorHandle constant_buffer_descriptor;
    } compute;

    struct Draw {
        static constexpr std::string_view NAME = "Draw"sv;

        ComPtr<ID3D12PipelineState> pipeline_state;

        GpuBufferHostCbv<DrawConstants> constant_buffer;
        GpuDescriptorHandle constant_buffer_descriptor;
    } draw;
};

}  // namespace fb::rain
