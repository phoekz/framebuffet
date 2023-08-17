#include <gpu/samplers.hlsli>
#include <demos/core.hlsli>

//
// Types
//

struct Constants {
    float4x4 transform;
    float pad[48];
};

struct Vertex {
    float3 position;
    float3 normal;
    float2 texcoord;
};

struct VertexOutput {
    float4 position: SV_Position;
    float3 normal: ATTRIBUTE0;
    float2 texcoord: ATTRIBUTE1;
};

//
// Bindings
//

struct Bindings {
    uint constants;
    uint vertices;
    uint texture;
};
ConstantBuffer<Bindings> g_bindings: register(b0);

//
// Entry points
//

VertexOutput draw_vs(FbVertexInput input) {
    ConstantBuffer<Constants> constants = ResourceDescriptorHeap[g_bindings.constants];
    StructuredBuffer<Vertex> vertices = ResourceDescriptorHeap[g_bindings.vertices];
    Vertex vertex = vertices[input.vertex_id];

    VertexOutput output;
    output.position = mul(constants.transform, float4(vertex.position, 1.0f));
    output.normal = vertex.normal;
    output.texcoord = vertex.texcoord;
    return output;
}

FbPixelOutput1 draw_ps(VertexOutput input) {
    Texture2D texture = ResourceDescriptorHeap[g_bindings.texture];
    SamplerState samp = SamplerDescriptorHeap[(uint)GpuSamplerType::LinearClamp];

    float3 color = texture.Sample(samp, input.texcoord).rgb;
    float3 light = normalize(float3(1.0f, 1.0f, 1.0f));
    float lighting = 0.25 + 0.75 * saturate(dot(input.normal, light));
    FbPixelOutput1 output;
    output.color = float4(color * lighting, 1.0f);
    return output;
}
