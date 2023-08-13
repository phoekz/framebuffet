#include "shaders/fibers.hlsli"

//
// Core
//

namespace core {

struct Constants {
    float4x4 transform;
    float delta_time;
    float light_speed;
    float light_range;
    float light_intensity;
};

struct Light {
    float3 position;
    float3 color;
    float speed_variation;
};

}  // namespace core

//
// Sim
//

namespace sim {

struct ComputeInput {
    uint3 id: SV_DispatchThreadID;
};

}  // namespace sim

//
// Light
//

namespace light {

struct Vertex {
    float3 position;
    float3 normal;
    float2 texcoord;
};

struct VertexInput {
    uint vertex_id: SV_VertexID;
    uint instance_id: SV_InstanceID;
};

struct VertexOutput {
    float4 position: SV_Position;
    float3 normal: ATTRIBUTE0;
    float2 texcoord: ATTRIBUTE1;
    float3 color: ATTRIBUTE2;
};

struct PixelOutput {
    float4 color: SV_Target;
};

}  // namespace light

//
// Plane
//

namespace plane {

struct Vertex {
    float3 position;
    float3 normal;
    float2 texcoord;
};

struct VertexInput {
    uint vertex_id: SV_VertexID;
};

struct VertexOutput {
    float4 position: SV_Position;
    float3 normal: ATTRIBUTE0;
    float2 texcoord: ATTRIBUTE1;
    float3 world_position: ATTRIBUTE2;
};

struct PixelOutput {
    float4 color: SV_Target;
};

}  // namespace plane

//
// Bindings
//

struct Bindings {
    uint constants;
    uint lights;
    uint vertices;
};
ConstantBuffer<Bindings> g_bindings: register(b0);

//
// Entry points
//

[numthreads(DISPATCH_SIZE, 1, 1)] void sim_cs(sim::ComputeInput input) {
    ConstantBuffer<core::Constants> constants = ResourceDescriptorHeap[g_bindings.constants];
    RWStructuredBuffer<core::Light> lights = ResourceDescriptorHeap[g_bindings.lights];
    uint id = input.id.x;

    core::Light light = lights[id];
    float3 velocity = float3(1.0f, 0.0f, 0.0f);
    velocity *= light.speed_variation;
    velocity *= constants.light_speed;
    velocity *= constants.delta_time;
    light.position += velocity;
    if (light.position.x > PLANE_RADIUS_X) {
        light.position.x = -PLANE_RADIUS_X;
    }
    lights[id] = light;
}

light::VertexOutput light_vs(light::VertexInput input) {
    ConstantBuffer<core::Constants> constants = ResourceDescriptorHeap[g_bindings.constants];
    StructuredBuffer<light::Vertex> vertices = ResourceDescriptorHeap[g_bindings.vertices];
    StructuredBuffer<core::Light> lights = ResourceDescriptorHeap[g_bindings.lights];
    light::Vertex vertex = vertices[input.vertex_id];
    core::Light light = lights[input.instance_id];

    float3 position = vertex.position * constants.light_range + light.position;

    light::VertexOutput output;
    output.position = mul(constants.transform, float4(position, 1.0f));
    output.normal = vertex.normal;
    output.texcoord = vertex.texcoord;
    output.color = light.color;
    return output;
}

light::PixelOutput light_ps(light::VertexOutput input) {
    light::PixelOutput output;
    output.color = float4(1.0f, 1.0f, 1.0f, 1.0f);
    return output;
}

plane::VertexOutput plane_vs(plane::VertexInput input) {
    ConstantBuffer<core::Constants> constants = ResourceDescriptorHeap[g_bindings.constants];
    StructuredBuffer<plane::Vertex> vertices = ResourceDescriptorHeap[g_bindings.vertices];
    plane::Vertex vertex = vertices[input.vertex_id];

    plane::VertexOutput output;
    output.position = mul(constants.transform, float4(vertex.position, 1.0f));
    output.normal = vertex.normal;
    output.texcoord = vertex.texcoord;
    output.world_position = vertex.position;
    return output;
}

float pow2(float x) {
    return x * x;
}

float pow4(float x) {
    float x2 = x * x;
    return x2 * x2;
}

plane::PixelOutput plane_ps(plane::VertexOutput input) {
    ConstantBuffer<core::Constants> constants = ResourceDescriptorHeap[g_bindings.constants];
    StructuredBuffer<core::Light> lights = ResourceDescriptorHeap[g_bindings.lights];

    float3 color = float3(0.02f, 0.02f, 0.02f);

    for (uint i = 0; i < LIGHT_COUNT; i++) {
        core::Light light = lights[i];

        float3 light_from_surface = light.position - input.world_position;
        float distance = max(0.001f, length(light_from_surface));
        float attenuation = saturate(1.0 - pow4(distance / constants.light_range)) / pow2(distance);
        float3 light_color = attenuation * light.color;
        color += constants.light_intensity * light_color;
    }

    plane::PixelOutput output;
    output.color = float4(color, 1.0f);
    return output;
}
