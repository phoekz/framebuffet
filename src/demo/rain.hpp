#pragma once

#include "../utils.hpp"
#include "../maths.hpp"
#include "../dx12.hpp"
#include "../buffers.hpp"
#include "../descriptors.hpp"
#include "../samplers.hpp"
#include "../root_signature.hpp"

namespace fb::rain {

constexpr uint32_t PARTICLE_COUNT = 4 * 1024;
constexpr uint32_t DISPATCH_SIZE = 128;
constexpr uint32_t DISPATCH_COUNT = PARTICLE_COUNT / DISPATCH_SIZE;

struct UpdateParams {
    float aspect_ratio;
    float delta_time;
};

struct ComputeConstants {
    float delta_time = 0.0f;
    float speed = 0.2f;
    float pad[62];
};

struct DrawConstants {
    Matrix transform;
    float pad[48];
};

struct Particle {
    Vector3 position;
};
static_assert(sizeof(Particle) == 12);

struct Demo {
    static constexpr const char* NAME = "Rain";

    Demo(Dx& dx);
    void update(const UpdateParams& params);
    void render(Dx& dx);

    GpuRootSignature root_signature;
    GpuDescriptors descriptors;

    GpuBuffer<Particle> particle_buffer;
    GpuDescriptorHandle particle_buffer_srv_descriptor;
    GpuDescriptorHandle particle_buffer_uav_descriptor;

    struct Compute {
        static constexpr const char* NAME = "Compute";

        ComPtr<ID3D12PipelineState> pipeline_state;

        GpuBuffer<ComputeConstants> constant_buffer;
        GpuDescriptorHandle constant_buffer_descriptor;
    } compute;

    struct Draw {
        static constexpr const char* NAME = "Draw";

        ComPtr<ID3D12PipelineState> pipeline_state;

        GpuBuffer<DrawConstants> constant_buffer;
        GpuDescriptorHandle constant_buffer_descriptor;

    } draw;

    ComPtr<ID3D12Resource> color_target;
    GpuDescriptorHandle color_target_descriptor;

    ComPtr<ID3D12Resource> depth_target;
    GpuDescriptorHandle depth_target_descriptor;
};

}  // namespace fb::rain
