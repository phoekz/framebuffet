cbuffer CardConstants: register(b0) {
    float2 card_position;
    float2 card_size;
};

cbuffer ConstantBuffer: register(b1) {
    float4x4 transform;
    float padding[48];
};

struct VertexInput {
    float2 position: POSITION;
    float2 texcoord: TEXCOORD0;
};

struct VertexOutput {
    float4 position: SV_POSITION;
    float2 texcoord: TEXCOORD0;
};

VertexOutput vs_main(VertexInput input) {
    float2 position = input.position * card_size + card_position;
    VertexOutput output;
    output.position = mul(transform, float4(position, 0.0f, 1.0f));
    output.texcoord = input.texcoord;
    return output;
}

struct PixelOutput {
    float4 color: SV_TARGET;
};

Texture2D g_texture: register(t0);
SamplerState g_sampler: register(s0);

PixelOutput ps_main(VertexOutput input) {
    PixelOutput output;
    output.color = g_texture.Sample(g_sampler, input.texcoord);
    // output.color = float4(1.0f, 0.0f, 0.0f, 1.0f);
    return output;
}
