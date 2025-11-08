#include <griddle/griddle.hlsli>
#include <kitchen/gpu/samplers.hlsli>
#include <kitchen/kcn/core.hlsli>

const ConstantBuffer<Bindings> g_bindings: register(b0);

struct VertexOutput {
    float4 position : SV_Position;
    float2 texcoord : ATTRIBUTE0;
};

VertexOutput vs(fb::VertexInput input) {
    const ConstantBuffer<Constants> constants = ResourceDescriptorHeap[g_bindings.constants];
    const StructuredBuffer<Vertex> vertices = ResourceDescriptorHeap[g_bindings.vertices];
    const Vertex vertex = vertices[input.vertex_id];

    VertexOutput output;
    output.position = mul(constants.transform, float4(vertex.position, 0.0f, 1.0f));
    output.texcoord = vertex.texcoord;
    return output;
}

fb::PixelOutput<1> ps(VertexOutput input) {
    const Texture2D<float4> texture = ResourceDescriptorHeap[g_bindings.texture];
    const SamplerState sampler = SamplerDescriptorHeap[g_bindings.sampler];

    const float3 color = texture.Sample(sampler, input.texcoord).rgb;

    fb::PixelOutput<1> output;
    output.color = float4(color, 1.0f);
    return output;
}
