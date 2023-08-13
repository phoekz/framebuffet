#include "shaders/samplers.hlsli"

//
// Types
//

struct Constants {
    float4x4 transform;
    float pad[48];
};

struct Vertex {
    float3 position;
    float3 normal;
    float2 texcoord;
};

struct VertexInput {
    uint index: SV_VertexID;
};

struct VertexOutput {
    float4 position: SV_Position;
    float3 normal: ATTRIBUTE0;
    float2 texcoord: ATTRIBUTE1;
    float3 direction: ATTRIBUTE2;
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
    uint texture;
};
ConstantBuffer<Bindings> g_bindings: register(b0);

//
// Entry points
//

VertexOutput vs_main(VertexInput input) {
    ConstantBuffer<Constants> constants = ResourceDescriptorHeap[g_bindings.constants];
    StructuredBuffer<Vertex> vertices = ResourceDescriptorHeap[g_bindings.vertices];
    Vertex vertex = vertices[input.index];
    float3 direction = vertex.position;

    VertexOutput output;
    output.position = mul(constants.transform, float4(vertex.position, 1.0f));
    output.position = output.position.xyww;  // Forces far plane.
    output.normal = vertex.normal;
    output.texcoord = vertex.texcoord;
    output.direction = direction;
    output.direction.z *= -1.0f;  // Handedness flip.
    return output;
}

PixelOutput ps_main(VertexOutput input) {
    TextureCube<float4> texture = ResourceDescriptorHeap[g_bindings.texture];
    SamplerState samp = SamplerDescriptorHeap[(uint)GpuSamplerType::LinearWrap];
    float3 color = texture.Sample(samp, input.direction).rgb;

    PixelOutput output;
    output.color = float4(color, 1.0f);
    return output;
}
