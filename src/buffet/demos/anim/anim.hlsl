#include <buffet/demos/anim/anim.hlsli>
#include <kitchen/gpu/samplers.hlsli>
#include <kitchen/graphics/core.hlsli>

ConstantBuffer<Bindings> g_bindings: register(b0);

struct VertexOutput {
    float4 position: SV_Position;
    float3 normal: ATTRIBUTE0;
    float2 texcoord: ATTRIBUTE1;
    float4 tangent: ATTRIBUTE2;
};

VertexOutput draw_vs(FbVertexInput input) {
    ConstantBuffer<Constants> constants = ResourceDescriptorHeap[g_bindings.constants];
    StructuredBuffer<FbSkinningVertex> vertices = ResourceDescriptorHeap[g_bindings.vertices];
    StructuredBuffer<float4x4> ibms = ResourceDescriptorHeap[g_bindings.joints_inverse_binds];
    StructuredBuffer<float4x4> gts = ResourceDescriptorHeap[g_bindings.joints_global_transforms];
    FbSkinningVertex vertex = vertices[input.vertex_id];
    uint j0 = vertex.joints[0];
    uint j1 = vertex.joints[1];
    uint j2 = vertex.joints[2];
    uint j3 = vertex.joints[3];
    float4 ws = vertex.weights;
    float4x4 m0 = ws[0] * mul(gts[j0], ibms[j0]);
    float4x4 m1 = ws[1] * mul(gts[j1], ibms[j1]);
    float4x4 m2 = ws[2] * mul(gts[j2], ibms[j2]);
    float4x4 m3 = ws[3] * mul(gts[j3], ibms[j3]);
    float4x4 transform = m0 + m1 + m2 + m3;

    VertexOutput output;
    output.position = mul(constants.transform, mul(transform, float4(vertex.position, 1.0f)));
    output.normal = vertex.normal;
    output.texcoord = vertex.texcoord;
    output.tangent = vertex.tangent;
    return output;
}

FbPixelOutput<1> draw_ps(VertexOutput input) {
    Texture2D texture = ResourceDescriptorHeap[g_bindings.texture];
    SamplerState sampler = SamplerDescriptorHeap[(uint)GpuSampler::LinearClamp];

    float3 color = texture.Sample(sampler, input.texcoord).rgb;
    float3 light = normalize(float3(1.0f, 1.0f, 1.0f));
    float lighting = 0.25 + 0.75 * saturate(dot(input.normal, light));
    FbPixelOutput<1> output;
    output.color = float4(color * lighting, 1.0f);
    return output;
}
