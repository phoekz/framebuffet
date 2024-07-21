#include <buffet/demos/anim/anim.hlsli>
#include <kitchen/gpu/samplers.hlsli>
#include <kitchen/graphics/core.hlsli>

ConstantBuffer<Bindings> g_bindings: register(b0);

struct VertexOutput {
    float4 position: SV_Position;
    float3 normal: ATTRIBUTE0;
    float2 texcoord: ATTRIBUTE1;
    float3 tangent: ATTRIBUTE2;
};

VertexOutput draw_vs(FbVertexInput input) {
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

    VertexOutput output;
    output.position = mul(constants.transform, mul(transform, float4(vertex.position, 1.0f)));
    output.normal = normalize(mul(transform, float4(vertex.normal, 0.0f)).xyz);
    output.texcoord = vertex.texcoord;
    output.tangent = normalize(mul(transform, float4(vertex.tangent.xyz, 0.0f)).xyz);
    return output;
}

FbPixelOutput<1> draw_ps(VertexOutput input) {
    const uint32_t4 color_uint = unpack_u8u32(g_bindings.color);
    const float3 color = float3(color_uint.rgb) / 255.0f;

    const float3 light = normalize(float3(-1.0f, 1.0f, 1.0f));
    const float lighting = 0.5 + 0.5 * saturate(dot(input.normal, light));

    FbPixelOutput<1> output;
    output.color = float4(color.rgb * lighting, 1.0f);
    return output;
}
