#include <gpu/samplers.hlsli>
#include <demos/core.hlsli>
#include <demos/tree/tree.hlsli>

struct Constants {
    float4x4 transform;
    float4x4 light_transform;
    float3 light_direction;
    float ambient_light;
    float pad[28];
};

struct Vertex {
    float3 position;
    float3 normal;
    float2 texcoord;
};

struct ShadowVertexOutput {
    float4 position: SV_Position;
};

struct DrawVertexOutput {
    float4 position: SV_Position;
    float3 normal: ATTRIBUTE0;
    float2 texcoord: ATTRIBUTE1;
    float4 shadow_coord: ATTRIBUTE2;
};

ConstantBuffer<Bindings> g_bindings: register(b0);

ShadowVertexOutput shadow_vs(FbVertexInput input) {
    ConstantBuffer<Constants> constants = ResourceDescriptorHeap[g_bindings.constants];
    StructuredBuffer<Vertex> vertices = ResourceDescriptorHeap[g_bindings.vertices];
    Vertex vertex = vertices[input.vertex_id];

    ShadowVertexOutput output;
    output.position = mul(constants.light_transform, float4(vertex.position, 1.0f));
    return output;
}

DrawVertexOutput draw_vs(FbVertexInput input) {
    ConstantBuffer<Constants> constants = ResourceDescriptorHeap[g_bindings.constants];
    StructuredBuffer<Vertex> vertices = ResourceDescriptorHeap[g_bindings.vertices];
    Vertex vertex = vertices[input.vertex_id];

    DrawVertexOutput output;
    output.position = mul(constants.transform, float4(vertex.position, 1.0f));
    output.normal = vertex.normal;
    output.texcoord = vertex.texcoord;
    output.shadow_coord = mul(constants.light_transform, float4(vertex.position, 1.0f));
    return output;
}

FbPixelOutput1 draw_ps(DrawVertexOutput input) {
    ConstantBuffer<Constants> constants = ResourceDescriptorHeap[g_bindings.constants];
    Texture2D texture = ResourceDescriptorHeap[g_bindings.texture];
    Texture2D shadow_texture = ResourceDescriptorHeap[g_bindings.shadow_texture];
    SamplerState samp = SamplerDescriptorHeap[(uint)GpuSamplerType::LinearClamp];
    SamplerComparisonState shadow_samp = SamplerDescriptorHeap[(uint)GpuSamplerType::Shadow];

    float3 shadow_coord = input.shadow_coord.xyz / input.shadow_coord.w;
    float2 shadow_texcoord = shadow_coord.xy * float2(0.5f, -0.5f) + 0.5f;
    float shadow_bias = 0.001f;
    float shadow = (float)shadow_texture.SampleCmpLevelZero(
        shadow_samp,
        shadow_texcoord.xy,
        shadow_coord.z - shadow_bias);

    float3 color = texture.Sample(samp, input.texcoord).rgb;
    float n_dot_l = shadow * saturate(dot(input.normal, constants.light_direction));
    float lighting = constants.ambient_light + (1.0 - constants.ambient_light) * n_dot_l;
    FbPixelOutput1 output;
    output.color = float4(color * lighting, 1.0f);
    return output;
}
