#include <buffet/demos/tree/tree.hlsli>
#include <kitchen/gpu/samplers.hlsli>
#include <kitchen/kcn/core.hlsli>

ConstantBuffer<Bindings> g_bindings: register(b0);

//
// Shadow
//

struct ShadowVertexOutput {
    float4 position: SV_Position;
};

ShadowVertexOutput shadow_vs(fb::VertexInput input) {
    const ConstantBuffer<Constants> constants = ResourceDescriptorHeap[g_bindings.constants];
    const StructuredBuffer<fb::Vertex> vertices = ResourceDescriptorHeap[g_bindings.vertices];
    const fb::Vertex vertex = vertices[input.vertex_id];

    ShadowVertexOutput output;
    output.position = mul(constants.light_transform, float4(vertex.position, 1.0f));
    return output;
}

//
// Draw
//

struct DrawVertexOutput {
    float4 position: SV_Position;
    float3 normal: ATTRIBUTE0;
    float2 texcoord: ATTRIBUTE1;
    float4 shadow_coord: ATTRIBUTE2;
};

DrawVertexOutput draw_vs(fb::VertexInput input) {
    const ConstantBuffer<Constants> constants = ResourceDescriptorHeap[g_bindings.constants];
    const StructuredBuffer<fb::Vertex> vertices = ResourceDescriptorHeap[g_bindings.vertices];
    const fb::Vertex vertex = vertices[input.vertex_id];

    DrawVertexOutput output;
    output.position = mul(constants.transform, float4(vertex.position, 1.0f));
    output.normal = vertex.normal;
    output.texcoord = vertex.texcoord;
    output.shadow_coord = mul(constants.light_transform, float4(vertex.position, 1.0f));
    return output;
}

fb::PixelOutput<1> draw_ps(DrawVertexOutput input) {
    const ConstantBuffer<Constants> constants = ResourceDescriptorHeap[g_bindings.constants];
    const Texture2D texture = ResourceDescriptorHeap[g_bindings.texture];
    const Texture2D shadow_texture = ResourceDescriptorHeap[g_bindings.shadow_texture];
    const SamplerState sampler = SamplerDescriptorHeap[(uint)GpuSampler::LinearClamp];
    const SamplerComparisonState shadow_sampler = SamplerDescriptorHeap[(uint)GpuSampler::Shadow];

    const float3 shadow_coord = input.shadow_coord.xyz / input.shadow_coord.w;
    const float2 shadow_texcoord = shadow_coord.xy * float2(0.5f, -0.5f) + 0.5f;
    const float shadow_bias = 0.001f;
    const float shadow = (float)shadow_texture.SampleCmpLevelZero(
        shadow_sampler,
        shadow_texcoord.xy,
        shadow_coord.z - shadow_bias
    );

    const float3 color = texture.Sample(sampler, input.texcoord).rgb;
    const float n_dot_l = shadow * saturate(dot(input.normal, constants.light_direction));
    const float lighting = constants.ambient_light + (1.0 - constants.ambient_light) * n_dot_l;

    fb::PixelOutput<1> output;
    output.color = float4(color * lighting, 1.0f);
    return output;
}
