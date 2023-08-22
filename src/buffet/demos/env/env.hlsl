#include <gpu/samplers.hlsli>
#include <demos/core.hlsli>
#include <demos/env/env.hlsli>

struct VertexOutput {
    float4 position: SV_Position;
    float3 normal: ATTRIBUTE0;
    float2 texcoord: ATTRIBUTE1;
    float3 direction: ATTRIBUTE2;
};

ConstantBuffer<Bindings> g_bindings: register(b0);

VertexOutput draw_vs(FbVertexInput input) {
    ConstantBuffer<Constants> constants = ResourceDescriptorHeap[g_bindings.constants];
    StructuredBuffer<FbVertex> vertices = ResourceDescriptorHeap[g_bindings.vertices];
    FbVertex vertex = vertices[input.vertex_id];
    float3 direction = vertex.position;

    VertexOutput output;
    output.position = mul(constants.transform, float4(vertex.position, 1.0f));
    output.position = output.position.xyww; // Forces far plane.
    output.normal = vertex.normal;
    output.texcoord = vertex.texcoord;
    output.direction = direction;
    output.direction.z *= -1.0f; // Handedness flip.
    return output;
}

FbPixelOutput1 draw_ps(VertexOutput input) {
    TextureCube<float4> texture = ResourceDescriptorHeap[g_bindings.texture];
    SamplerState samp = SamplerDescriptorHeap[(uint)GpuSamplerType::LinearWrap];
    float3 color = texture.Sample(samp, input.direction).rgb;

    FbPixelOutput1 output;
    output.color = float4(color, 1.0f);
    return output;
}
