#include <gpu/samplers.hlsli>
#include <demos/core.hlsli>

//
// Types
//

struct Constants {
    float4x4 transform;
};

struct Vertex {
    float2 position;
    float2 texcoord;
    uint8_t4_packed color;
};

struct VertexOutput {
    float4 position: SV_Position;
    float4 color: ATTRIBUTE0;
    float2 texcoord: ATTRIBUTE1;
};

//
// Bindings
//

struct Bindings {
    uint constants;
    uint vertices;
    uint base_vertex;
    uint texture;
};
ConstantBuffer<Bindings> g_bindings: register(b0);

//
// Entry points
//

VertexOutput draw_vs(FbVertexInput input) {
    ConstantBuffer<Constants> constants = ResourceDescriptorHeap[g_bindings.constants];
    StructuredBuffer<Vertex> vertices = ResourceDescriptorHeap[g_bindings.vertices];
    Vertex vertex = vertices[g_bindings.base_vertex + input.vertex_id];

    VertexOutput output;
    output.position = mul(constants.transform, float4(vertex.position, 0.0f, 1.0f));
    output.color = float4(unpack_u8u32(vertex.color)) / 255.0;
    output.texcoord = vertex.texcoord;
    return output;
};

FbPixelOutput1 draw_ps(VertexOutput input) {
    Texture2D texture = ResourceDescriptorHeap[g_bindings.texture];
    SamplerState samp = SamplerDescriptorHeap[(uint)GpuSamplerType::LinearWrap];

    FbPixelOutput1 output;
    output.color = input.color * texture.Sample(samp, input.texcoord);
    return output;
};