//
// Types
//

struct Constants {
    float4x4 transform;
    float4x4 particle_transform;
};

struct Vertex {
    float3 position;
    float2 texcoord;
};

struct Particle {
    float3 position;
};

struct VertexInput {
    uint vertex_id: SV_VertexID;
    uint instance_id: SV_InstanceID;
};

struct VertexOutput {
    float4 position: SV_Position;
    float2 texcoord: ATTRIBUTE0;
};

struct PixelOutput {
    float4 color: SV_Target;
};

//
// Bindings
//

struct Bindings {
    uint constants;
    uint vertices;
    uint particles;
};
ConstantBuffer<Bindings> g_bindings: register(b0);

//
// Entry points
//

VertexOutput vs_main(VertexInput input) {
    ConstantBuffer<Constants> constants = ResourceDescriptorHeap[g_bindings.constants];
    StructuredBuffer<Vertex> vertices = ResourceDescriptorHeap[g_bindings.vertices];
    StructuredBuffer<Particle> particles = ResourceDescriptorHeap[g_bindings.particles];
    Vertex vertex = vertices[input.vertex_id];
    Particle particle = particles[input.instance_id];

    float3 position;
    position = vertex.position;
    position = mul((float3x3)constants.particle_transform, position);
    position = position + particle.position;

    VertexOutput output;
    output.position = mul(constants.transform, float4(position, 1.0f));
    output.texcoord = vertex.texcoord;
    return output;
}

PixelOutput ps_main(VertexOutput input) {
    float distance = saturate(length(2.0f * input.texcoord - 1.0f));
    float gradient = pow(1.0f - distance, 3.0f);
    float3 color = gradient.xxx;

    PixelOutput output;
    output.color = float4(color, gradient);
    return output;
}
