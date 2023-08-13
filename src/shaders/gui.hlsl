#include "shaders/samplers.hlsli"

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

struct VertexInput {
    uint index: SV_VertexID;
};

struct VertexOutput {
    float4 position: SV_Position;
    float4 color: ATTRIBUTE0;
    float2 texcoord: ATTRIBUTE1;
};

struct PixelOutput {
    float4 color: SV_Target;
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

VertexOutput vs_main(VertexInput input) {
    ConstantBuffer<Constants> constants = ResourceDescriptorHeap[g_bindings.constants];
    StructuredBuffer<Vertex> vertices = ResourceDescriptorHeap[g_bindings.vertices];
    Vertex vertex = vertices[g_bindings.base_vertex + input.index];

    VertexOutput output;
    output.position = mul(constants.transform, float4(vertex.position, 0.0f, 1.0f));
    output.color = float4(unpack_u8u32(vertex.color)) / 255.0;
    output.texcoord = vertex.texcoord;
    return output;
};

PixelOutput ps_main(VertexOutput input) {
    Texture2D texture = ResourceDescriptorHeap[g_bindings.texture];
    SamplerState samp = SamplerDescriptorHeap[(uint)GpuSamplerType::LinearWrap];

    PixelOutput output;
    output.color = input.color * texture.Sample(samp, input.texcoord);
    return output;
};
