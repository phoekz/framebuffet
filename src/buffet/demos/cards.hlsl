#include <gpu/samplers.hlsli>
#include <demos/core.hlsli>

//
// Types
//

struct Constants {
    float4x4 transform;
    float pad[48];
};

struct Card {
    float2 position;
    float2 size;
};

struct Vertex {
    float2 position;
    float2 texcoord;
};

struct VertexOutput {
    float4 position: SV_Position;
    float2 texcoord: ATTRIBUTE0;
};

//
// Bindings
//

struct Bindings {
    uint card_index;
    uint constants;
    uint cards;
    uint vertices;
    uint texture;
};
ConstantBuffer<Bindings> g_bindings: register(b0);

//
// Entry points
//

VertexOutput draw_vs(FbVertexInput input) {
    ConstantBuffer<Constants> constants = ResourceDescriptorHeap[g_bindings.constants];
    StructuredBuffer<Card> cards = ResourceDescriptorHeap[g_bindings.cards];
    Card card = cards[g_bindings.card_index];
    StructuredBuffer<Vertex> vertices = ResourceDescriptorHeap[g_bindings.vertices];
    Vertex vertex = vertices[input.vertex_id];

    float2 position = vertex.position * card.size + card.position;
    VertexOutput output;
    output.position = mul(constants.transform, float4(position, 0.0f, 1.0f));
    output.texcoord = vertex.texcoord;
    return output;
}

FbPixelOutput1 draw_ps(VertexOutput input) {
    Texture2D texture = ResourceDescriptorHeap[g_bindings.texture];
    SamplerState samp = SamplerDescriptorHeap[(uint)GpuSamplerType::LinearClamp];

    FbPixelOutput1 output;
    output.color = texture.Sample(samp, input.texcoord);
    return output;
}