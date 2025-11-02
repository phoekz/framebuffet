#include <kitchen/kcn/core.hlsli>
#include <kitchen/kcn/debug_draw.hlsli>

struct Constants {
    float4x4 transform;
    float pad[48];
};

struct Vertex {
    float3 position;
    uint8_t4_packed color;
};

struct VertexOutput {
    float4 position : SV_Position;
    float4 color : ATTRIBUTE0;
};

const ConstantBuffer<Bindings> g_bindings: register(b0);

VertexOutput draw_vs(fb::VertexInput input) {
    const ConstantBuffer<Constants> constants = ResourceDescriptorHeap[g_bindings.constants];
    const StructuredBuffer<Vertex> vertices = ResourceDescriptorHeap[g_bindings.vertices];
    const Vertex vertex = vertices[input.vertex_id];

    VertexOutput output;
    output.position = mul(constants.transform, float4(vertex.position, 1.0f));
    output.color = float4(unpack_u8u32(vertex.color)) / 255.0;
    return output;
}

fb::PixelOutput<1> draw_ps(VertexOutput input) {
    fb::PixelOutput<1> output;
    output.color = input.color;
    return output;
}
