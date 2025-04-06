#include <buffet/demos/anim/anim.hlsli>
#include <kitchen/gpu/samplers.hlsli>
#include <kitchen/kcn/core.hlsli>

const ConstantBuffer<Bindings> g_bindings: register(b0);

//
// Shadow
//

struct ShadowVertexOutput {
    float4 position: SV_Position;
};

ShadowVertexOutput shadow_vs(fb::VertexInput input) {
    const ConstantBuffer<Constants> constants = ResourceDescriptorHeap[g_bindings.constants];
    const StructuredBuffer<fb::SkinningVertex> vertices =
        ResourceDescriptorHeap[g_bindings.vertices];
    const StructuredBuffer<float4x4> skinning_matrices =
        ResourceDescriptorHeap[g_bindings.skinning_matrices];

    const fb::SkinningVertex vertex = vertices[input.vertex_id];
    const float4 weights = vertex.weights;
    const float4x4 m0 = weights[0] * skinning_matrices[vertex.joints[0]];
    const float4x4 m1 = weights[1] * skinning_matrices[vertex.joints[1]];
    const float4x4 m2 = weights[2] * skinning_matrices[vertex.joints[2]];
    const float4x4 m3 = weights[3] * skinning_matrices[vertex.joints[3]];
    const float4x4 transform = m0 + m1 + m2 + m3;

    ShadowVertexOutput output;
    output.position = mul(constants.light_transform, mul(transform, float4(vertex.position, 1.0f)));
    return output;
}

//
// Ground
//

struct GroundVertexOutput {
    float4 position: SV_Position;
    float3 normal: ATTRIBUTE0;
    float2 texcoord: ATTRIBUTE1;
    float3 world_position: ATTRIBUTE2;
    float4 shadow_coord: ATTRIBUTE3;
};

GroundVertexOutput ground_vs(fb::VertexInput input) {
    const ConstantBuffer<Constants> constants = ResourceDescriptorHeap[g_bindings.constants];
    const StructuredBuffer<fb::Vertex> vertices = ResourceDescriptorHeap[g_bindings.vertices];
    const fb::Vertex vertex = vertices[input.vertex_id];

    GroundVertexOutput output;
    output.position = mul(constants.transform, float4(vertex.position, 1.0f));
    output.normal = vertex.normal;
    output.texcoord = vertex.texcoord;
    output.world_position = vertex.position;
    output.shadow_coord = mul(constants.light_transform, float4(vertex.position, 1.0f));
    return output;
}

fb::PixelOutput<1> ground_ps(GroundVertexOutput input) {
    const Texture2D<float3> texture = ResourceDescriptorHeap[g_bindings.texture];
    const SamplerState sampler = SamplerDescriptorHeap[(uint)GpuSampler::PointWrap];
    const Texture2D<float> shadow_texture = ResourceDescriptorHeap[g_bindings.shadow_texture];
    const SamplerComparisonState shadow_sampler = SamplerDescriptorHeap[(uint)GpuSampler::Shadow];
    const float texture_scroll = g_bindings.texture_scroll;

    const float3 shadow_coord = input.shadow_coord.xyz / input.shadow_coord.w;
    const float2 shadow_texcoord = shadow_coord.xy * float2(0.5f, -0.5f) + 0.5f;
    const float shadow_bias = 0.001f;
    const float shadow = shadow_texture.SampleCmpLevelZero(
        shadow_sampler,
        shadow_texcoord.xy,
        shadow_coord.z - shadow_bias
    );

    const float3 light = normalize(float3(-1.0f, 1.0f, 1.0f));
    const float lighting = 0.5 + 0.5 * saturate(shadow * dot(input.normal, light));

    const float3 color = texture.Sample(sampler, input.texcoord + float2(0.0f, texture_scroll));

    const float3 world_position = input.world_position;
    const float distance_to_origin = saturate(length(world_position) / 8.0f);

    fb::PixelOutput<1> output;
    output.color = float4(lerp(lighting * color, 0.75f.xxx, distance_to_origin), 1.0f);
    return output;
}

//
// Animation
//

struct AnimVertexOutput {
    float4 position: SV_Position;
    float3 normal: ATTRIBUTE0;
    float2 texcoord: ATTRIBUTE1;
    float3 tangent: ATTRIBUTE2;
    float4 shadow_coord: ATTRIBUTE3;
};

AnimVertexOutput anim_vs(fb::VertexInput input) {
    const ConstantBuffer<Constants> constants = ResourceDescriptorHeap[g_bindings.constants];
    const StructuredBuffer<fb::SkinningVertex> vertices =
        ResourceDescriptorHeap[g_bindings.vertices];
    const StructuredBuffer<float4x4> skinning_matrices =
        ResourceDescriptorHeap[g_bindings.skinning_matrices];

    const fb::SkinningVertex vertex = vertices[input.vertex_id];
    const float4 weights = vertex.weights;
    const float4x4 m0 = weights[0] * skinning_matrices[vertex.joints[0]];
    const float4x4 m1 = weights[1] * skinning_matrices[vertex.joints[1]];
    const float4x4 m2 = weights[2] * skinning_matrices[vertex.joints[2]];
    const float4x4 m3 = weights[3] * skinning_matrices[vertex.joints[3]];
    const float4x4 transform = m0 + m1 + m2 + m3;

    const float4 local_vertex = mul(transform, float4(vertex.position, 1.0f));

    AnimVertexOutput output;
    output.position = mul(constants.transform, local_vertex);
    output.normal = normalize(mul(transform, float4(vertex.normal, 0.0f)).xyz);
    output.texcoord = vertex.texcoord;
    output.tangent = normalize(mul(transform, float4(vertex.tangent.xyz, 0.0f)).xyz);
    output.shadow_coord = mul(constants.light_transform, local_vertex);
    return output;
}

fb::PixelOutput<1> anim_ps(AnimVertexOutput input) {
    const Texture2D shadow_texture = ResourceDescriptorHeap[g_bindings.shadow_texture];
    const SamplerComparisonState shadow_sampler = SamplerDescriptorHeap[(uint)GpuSampler::Shadow];
    const uint32_t4 color_uint = unpack_u8u32(g_bindings.color);
    const float3 color = float3(color_uint.rgb) / 255.0f;

    const float3 shadow_coord = input.shadow_coord.xyz / input.shadow_coord.w;
    const float2 shadow_texcoord = shadow_coord.xy * float2(0.5f, -0.5f) + 0.5f;
    const float shadow_bias = 0.001f;
    const float shadow = (float)shadow_texture.SampleCmpLevelZero(
        shadow_sampler,
        shadow_texcoord.xy,
        shadow_coord.z - shadow_bias
    );

    const float3 light = normalize(float3(-1.0f, 1.0f, 1.0f));
    const float lighting = 0.5 + 0.5 * saturate(shadow * dot(input.normal, light));

    fb::PixelOutput<1> output;
    output.color = float4(color.rgb * lighting, 1.0f);
    return output;
}
