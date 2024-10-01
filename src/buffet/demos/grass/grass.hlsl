#include <buffet/demos/grass/grass.hlsli>
#include <kitchen/gpu/samplers.hlsli>
#include <kitchen/kcn/core.hlsli>

const ConstantBuffer<Bindings> g_bindings: register(b0);

struct VertexOutput {
    float4 position: SV_Position;
    float3 normal: ATTRIBUTE0;
    float2 texcoord: ATTRIBUTE1;
};

VertexOutput draw_vs(fb::VertexInput input) {
    const ConstantBuffer<Constants> constants = ResourceDescriptorHeap[g_bindings.constants];
    const StructuredBuffer<fb::Vertex> vertices = ResourceDescriptorHeap[g_bindings.vertices];
    const fb::Vertex vertex = vertices[input.vertex_id];

    VertexOutput output;
    output.position = mul(constants.transform, float4(vertex.position, 1.0f));
    output.normal = vertex.normal;
    output.texcoord = vertex.texcoord;
    return output;
}

fb::PixelOutput<1> draw_naive_ps(VertexOutput input) {
    const ConstantBuffer<Constants> constants = ResourceDescriptorHeap[g_bindings.constants];
    const float alpha_cutoff = constants.alpha_cutoff;
    const Texture2D texture = ResourceDescriptorHeap[g_bindings.texture];
    const SamplerState sampler = SamplerDescriptorHeap[(uint)GpuSampler::LinearClamp];

    const float4 color_alpha = texture.Sample(sampler, input.texcoord);
    const float3 color = color_alpha.rgb;
    const float alpha = color_alpha.a;

    clip(alpha - alpha_cutoff);

    fb::PixelOutput<1> output;
    output.color = float4(color, alpha);
    return output;
}

fb::PixelOutput<1> draw_atoc_ps(VertexOutput input) {
    const ConstantBuffer<Constants> constants = ResourceDescriptorHeap[g_bindings.constants];
    const float alpha_cutoff = constants.alpha_cutoff;
    const Texture2D texture = ResourceDescriptorHeap[g_bindings.texture];
    const SamplerState sampler = SamplerDescriptorHeap[(uint)GpuSampler::LinearClamp];

    const float4 color_alpha = texture.Sample(sampler, input.texcoord);
    const float3 color = color_alpha.rgb;
    const float alpha = color_alpha.a;

    const float epsilon = 1.0 / 8192.0;
    const float cut_alpha = (alpha - alpha_cutoff) / max(fwidth(alpha), epsilon) + 0.5;

    fb::PixelOutput<1> output;
    output.color = float4(color, cut_alpha);
    return output;
}
