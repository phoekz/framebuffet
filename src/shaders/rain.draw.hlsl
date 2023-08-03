//
// Types
//

struct Constants {
    float4x4 transform;
};

struct Vertex {
    float3 position;
};

struct VertexInput {
    uint index: SV_VertexID;
};

struct VertexOutput {
    float4 position: SV_POSITION;
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
    return output;
}

PixelOutput ps_main(VertexOutput input) {
    PixelOutput output;
    output.color = float4(1.0f, 1.0f, 1.0f, 1.0f);
    return output;
}
