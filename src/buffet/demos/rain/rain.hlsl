#include <buffet/demos/rain/rain.hlsli>
#include <kitchen/gpu/samplers.hlsli>
#include <kitchen/kcn/core.hlsli>

const ConstantBuffer<Bindings> g_bindings: register(b0);

FB_ATTRIBUTE(numthreads, DISPATCH_X, DISPATCH_Y, DISPATCH_Z)
void sim_cs(fb::ComputeInput input) {
    const ConstantBuffer<Constants> constants = ResourceDescriptorHeap[g_bindings.constants];
    const RWStructuredBuffer<Particle> particles = ResourceDescriptorHeap[g_bindings.particles];

    const uint i = input.dispatch_thread_id.x;
    Particle p = particles[i];
    p.position.y -= constants.speed * constants.delta_time;
    if (p.position.y < -1.0f) {
        p.position.y = 1.0f;
    }
    particles[i] = p;
}

struct VertexOutput {
    float4 position: SV_Position;
    float2 texcoord: ATTRIBUTE0;
};

VertexOutput draw_vs(fb::VertexInput input) {
    const ConstantBuffer<Constants> constants = ResourceDescriptorHeap[g_bindings.constants];
    const StructuredBuffer<Vertex> vertices = ResourceDescriptorHeap[g_bindings.vertices];
    const StructuredBuffer<Particle> particles = ResourceDescriptorHeap[g_bindings.particles];
    const Vertex vertex = vertices[input.vertex_id];
    const Particle particle = particles[input.instance_id];

    float3 position;
    position = vertex.position;
    position = mul((float3x3)constants.particle_transform, position);
    position = position + particle.position;

    VertexOutput output;
    output.position = mul(constants.transform, float4(position, 1.0f));
    output.texcoord = vertex.texcoord;
    return output;
}

fb::PixelOutput<1> draw_ps(VertexOutput input) {
    const float distance = saturate(length(2.0f * input.texcoord - 1.0f));
    const float gradient = pow(1.0f - distance, 3.0f);
    const float3 color = gradient.xxx;

    fb::PixelOutput<1> output;
    output.color = float4(color, gradient);
    return output;
}
