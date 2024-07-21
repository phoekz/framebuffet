#include <buffet/demos/anim/anim.hlsli>
#include <kitchen/gpu/samplers.hlsli>
#include <kitchen/graphics/core.hlsli>

ConstantBuffer<Bindings> g_bindings: register(b0);

//
// Ground
//

struct GroundVertexOutput {
    float4 position: SV_Position;
    float3 normal: ATTRIBUTE0;
    float2 texcoord: ATTRIBUTE1;
    float3 world_position: ATTRIBUTE2;
};

GroundVertexOutput ground_vs(FbVertexInput input) {
    ConstantBuffer<Constants> constants = ResourceDescriptorHeap[g_bindings.constants];
    StructuredBuffer<FbVertex> vertices = ResourceDescriptorHeap[g_bindings.vertices];
    FbVertex vertex = vertices[input.vertex_id];

    GroundVertexOutput output;
    output.position = mul(constants.transform, float4(vertex.position, 1.0f));
    output.normal = vertex.normal;
    output.texcoord = vertex.texcoord;
    output.world_position = vertex.position;
    return output;
}

FbPixelOutput<1> ground_ps(GroundVertexOutput input) {
    Texture2D<float4> texture = ResourceDescriptorHeap[g_bindings.texture];
    SamplerState sampler = SamplerDescriptorHeap[(uint)GpuSampler::PointWrap];
    const float texture_scroll = g_bindings.texture_scroll;

    const float3 light = normalize(float3(-1.0f, 1.0f, 1.0f));
    const float lighting = 0.5 + 0.5 * saturate(dot(input.normal, light));

    const float3 color = texture.Sample(sampler, input.texcoord + float2(0.0f, texture_scroll)).rgb;

    const float3 world_position = input.world_position;
    const float distance_to_origin = saturate(length(world_position) / 8.0f);
    (void)distance_to_origin;

    FbPixelOutput<1> output;
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
};

AnimVertexOutput anim_vs(FbVertexInput input) {
    ConstantBuffer<Constants> constants = ResourceDescriptorHeap[g_bindings.constants];
    StructuredBuffer<FbSkinningVertex> vertices = ResourceDescriptorHeap[g_bindings.vertices];
    StructuredBuffer<float4x4> skinning_matrices =
        ResourceDescriptorHeap[g_bindings.skinning_matrices];

    const FbSkinningVertex vertex = vertices[input.vertex_id];
    const float4 weights = vertex.weights;
    const float4x4 m0 = weights[0] * skinning_matrices[vertex.joints[0]];
    const float4x4 m1 = weights[1] * skinning_matrices[vertex.joints[1]];
    const float4x4 m2 = weights[2] * skinning_matrices[vertex.joints[2]];
    const float4x4 m3 = weights[3] * skinning_matrices[vertex.joints[3]];
    const float4x4 transform = m0 + m1 + m2 + m3;

    AnimVertexOutput output;
    output.position = mul(constants.transform, mul(transform, float4(vertex.position, 1.0f)));
    output.normal = normalize(mul(transform, float4(vertex.normal, 0.0f)).xyz);
    output.texcoord = vertex.texcoord;
    output.tangent = normalize(mul(transform, float4(vertex.tangent.xyz, 0.0f)).xyz);
    return output;
}

FbPixelOutput<1> anim_ps(AnimVertexOutput input) {
    const uint32_t4 color_uint = unpack_u8u32(g_bindings.color);
    const float3 color = float3(color_uint.rgb) / 255.0f;

    const float3 light = normalize(float3(-1.0f, 1.0f, 1.0f));
    const float lighting = 0.5 + 0.5 * saturate(dot(input.normal, light));

    FbPixelOutput<1> output;
    output.color = float4(color.rgb * lighting, 1.0f);
    return output;
}
