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
    uint8_t4_packed color;
};

struct VertexOutput {
    float4 position: SV_Position;
    float4 color: ATTRIBUTE0;
};

//
// Bindings
//

struct Bindings {
    uint constants;
    uint vertices;
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
    output.color = float4(unpack_u8u32(vertex.color)) / 255.0;
    return output;
}

FbPixelOutput1 draw_ps(VertexOutput input) {
    FbPixelOutput1 output;
    output.color = input.color;
    return output;
}
