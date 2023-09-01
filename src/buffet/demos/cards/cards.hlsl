#include <buffet/demos/cards/cards.hlsli>
#include <kitchen/gpu/samplers.hlsli>
#include <kitchen/graphics/core.hlsli>

ConstantBuffer<Bindings> g_bindings: register(b0);

struct VertexOutput {
    float4 position: SV_Position;
    float2 texcoord: ATTRIBUTE0;
};

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

FbPixelOutput<1> draw_ps(VertexOutput input) {
    Texture2D texture = ResourceDescriptorHeap[g_bindings.texture];
    SamplerState sampler = SamplerDescriptorHeap[(uint)GpuSampler::LinearClamp];

    FbPixelOutput<1> output;
    output.color = texture.Sample(sampler, input.texcoord);
    return output;
}
