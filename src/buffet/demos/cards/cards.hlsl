#include <buffet/demos/cards/cards.hlsli>
#include <kitchen/gpu/samplers.hlsli>
#include <kitchen/kcn/core.hlsli>

//
// Background
//

ConstantBuffer<BackgroundBindings> g_background_bindings: register(b0);

struct BackgroundVertexOutput {
    float4 position: SV_Position;
    float2 texcoord: ATTRIBUTE0;
};

BackgroundVertexOutput background_vs(fb::VertexInput input) {
    BackgroundVertexOutput output;
    fb::fullscreen_triangle(input.vertex_id, output.position, output.texcoord);
    return output;
}

fb::PixelOutput<1> background_ps(BackgroundVertexOutput input) {
    ConstantBuffer<Constants> constants = ResourceDescriptorHeap[g_background_bindings.constants];
    Texture2D texture = ResourceDescriptorHeap[g_background_bindings.texture];
    SamplerState sampler = SamplerDescriptorHeap[(uint)GpuSampler::LinearClamp];

    const float zoom = constants.zoom_factor;
    const float2 zoom_begin = zoom.xx;
    const float2 zoom_end = (1.0f - zoom).xx;
    const float2 zoom_texcoord = lerp(zoom_begin, zoom_end, input.texcoord);
    const float lod_bias = constants.lod_bias;

    fb::PixelOutput<1> output;
    output.color = texture.SampleBias(sampler, zoom_texcoord, lod_bias);
    return output;
}

//
// Draw
//

ConstantBuffer<DrawBindings> g_draw_bindings: register(b0);

struct DrawVertexOutput {
    float4 position: SV_Position;
    float2 texcoord: ATTRIBUTE0;
};

DrawVertexOutput draw_vs(fb::VertexInput input) {
    ConstantBuffer<Constants> constants = ResourceDescriptorHeap[g_draw_bindings.constants];
    StructuredBuffer<Card> cards = ResourceDescriptorHeap[g_draw_bindings.cards];
    Card card = cards[g_draw_bindings.card_index];
    StructuredBuffer<Vertex> vertices = ResourceDescriptorHeap[g_draw_bindings.vertices];
    Vertex vertex = vertices[input.vertex_id];

    float2 position = vertex.position * card.size + card.position;
    DrawVertexOutput output;
    output.position = mul(constants.transform, float4(position, 0.0f, 1.0f));
    output.texcoord = vertex.texcoord;
    return output;
}

fb::PixelOutput<1> draw_ps(DrawVertexOutput input) {
    Texture2D texture = ResourceDescriptorHeap[g_draw_bindings.texture];
    SamplerState sampler = SamplerDescriptorHeap[(uint)GpuSampler::LinearClamp];

    fb::PixelOutput<1> output;
    output.color = texture.Sample(sampler, input.texcoord);
    return output;
}
