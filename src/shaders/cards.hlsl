#include "shaders/samplers.hlsli"

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

struct VertexInput {
    float2 position: POSITION;
    float2 texcoord: TEXCOORD0;
};

struct VertexOutput {
    float4 position: SV_POSITION;
    float2 texcoord: TEXCOORD0;
};

struct PixelOutput {
    float4 color: SV_TARGET;
};

//
// Bindings
//

struct Bindings {
    uint card_index;
    uint constants;
    uint cards;
    uint texture;
};
ConstantBuffer<Bindings> g_bindings: register(b0);

//
// Entry points
//

VertexOutput vs_main(VertexInput input) {
    ConstantBuffer<Constants> constants = ResourceDescriptorHeap[g_bindings.constants];
    StructuredBuffer<Card> cards = ResourceDescriptorHeap[g_bindings.cards];
    Card card = cards[g_bindings.card_index];  // Todo:

    float2 position = input.position * card.size + card.position;
    VertexOutput output;
    output.position = mul(constants.transform, float4(position, 0.0f, 1.0f));
    output.texcoord = input.texcoord;
    return output;
}

PixelOutput ps_main(VertexOutput input) {
    Texture2D texture = ResourceDescriptorHeap[g_bindings.texture];
    SamplerState samp = SamplerDescriptorHeap[(uint)GpuSamplerType::LinearClamp];

    PixelOutput output;
    output.color = texture.Sample(samp, input.texcoord);
    return output;
}
