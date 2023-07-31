//
// Types
//

struct CardConstants {
    float2 position;
    float2 size;
};

struct Constants {
    float4x4 transform;
    float pad[48];
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
// I/O
//

ConstantBuffer<CardConstants> g_card_constants: register(b0);
ConstantBuffer<Constants> g_constants: register(b1);
Texture2D g_texture: register(t0);
SamplerState g_sampler: register(s0);

//
// Entry points
//

VertexOutput vs_main(VertexInput input) {
    float2 position = input.position * g_card_constants.size + g_card_constants.position;
    VertexOutput output;
    output.position = mul(g_constants.transform, float4(position, 0.0f, 1.0f));
    output.texcoord = input.texcoord;
    return output;
}

PixelOutput ps_main(VertexOutput input) {
    PixelOutput output;
    output.color = g_texture.Sample(g_sampler, input.texcoord);
    return output;
}
