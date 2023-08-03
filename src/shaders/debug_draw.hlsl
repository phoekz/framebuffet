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

struct VertexInput {
    uint index: SV_VertexID;
};

struct VertexOutput {
    float4 position: SV_POSITION;
    float4 color: COLOR0;
};

struct PixelOutput {
    float4 color: SV_TARGET;
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

VertexOutput vs_main(VertexInput input) {
    ConstantBuffer<Constants> constants = ResourceDescriptorHeap[g_bindings.constants];
    StructuredBuffer<Vertex> vertices = ResourceDescriptorHeap[g_bindings.vertices];
    Vertex vertex = vertices[input.index];

    VertexOutput output;
    output.position = mul(constants.transform, float4(vertex.position, 1.0f));
    output.color = float4(unpack_u8u32(vertex.color)) / 255.0;
    return output;
}

PixelOutput ps_main(VertexOutput input) {
    PixelOutput output;
    output.color = input.color;
    return output;
}
