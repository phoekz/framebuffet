#include <buffet/demos/grass/grass.hlsli>
#include <kitchen/gpu/samplers.hlsli>
#include <kitchen/graphics/core.hlsli>

ConstantBuffer<Bindings> g_bindings: register(b0);

struct VertexOutput {
    float4 position: SV_Position;
    float3 normal: ATTRIBUTE0;
    float2 texcoord: ATTRIBUTE1;
};

VertexOutput draw_vs(FbVertexInput input) {
    ConstantBuffer<Constants> constants = ResourceDescriptorHeap[g_bindings.constants];
    StructuredBuffer<FbVertex> vertices = ResourceDescriptorHeap[g_bindings.vertices];
    FbVertex vertex = vertices[input.vertex_id];

    VertexOutput output;
    output.position = mul(constants.transform, float4(vertex.position, 1.0f));
    output.normal = vertex.normal;
    output.texcoord = vertex.texcoord;
    return output;
}

FbPixelOutput<1> draw_naive_ps(VertexOutput input) {
    ConstantBuffer<Constants> constants = ResourceDescriptorHeap[g_bindings.constants];
    float alpha_cutoff = constants.alpha_cutoff;
    Texture2D texture = ResourceDescriptorHeap[g_bindings.texture];
    SamplerState samp = SamplerDescriptorHeap[(uint)GpuSampler::LinearClamp];

    float4 color_alpha = texture.Sample(samp, input.texcoord);
    float3 color = color_alpha.rgb;
    float alpha = color_alpha.a;

    clip(alpha - alpha_cutoff);

    FbPixelOutput<1> output;
    output.color = float4(color, alpha);
    return output;
}

FbPixelOutput<1> draw_atoc_ps(VertexOutput input) {
    ConstantBuffer<Constants> constants = ResourceDescriptorHeap[g_bindings.constants];
    float alpha_cutoff = constants.alpha_cutoff;
    Texture2D texture = ResourceDescriptorHeap[g_bindings.texture];
    SamplerState samp = SamplerDescriptorHeap[(uint)GpuSampler::LinearClamp];

    float4 color_alpha = texture.Sample(samp, input.texcoord);
    float3 color = color_alpha.rgb;
    float alpha = color_alpha.a;

    float epsilon = 1.0 / 8192.0;
    alpha = (alpha - alpha_cutoff) / max(fwidth(alpha), epsilon) + 0.5;

    FbPixelOutput<1> output;
    output.color = float4(color, alpha);
    return output;
}
