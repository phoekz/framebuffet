cbuffer ConstantBuffer: register(b0) {
    float4x4 transform;
    float padding[48];
};

struct VertexInput {
    float2 position: POSITION;
    float4 color: COLOR0;
    float2 texcoord: TEXCOORD0;
};

struct VertexOutput {
    float4 position: SV_POSITION;
    float4 color: COLOR0;
    float2 texcoord: TEXCOORD0;
};

VertexOutput vertex_shader(VertexInput input) {
    VertexOutput output;
    output.position = mul(transform, float4(input.position, 0.0f, 1.0f));
    output.color = input.color;
    output.texcoord = input.texcoord;
    return output;
};

struct PixelOutput {
    float4 color: SV_TARGET;
};

Texture2D g_texture: register(t0);
SamplerState g_sampler: register(s0);

PixelOutput pixel_shader(VertexOutput input) {
    PixelOutput output;
    output.color = input.color * g_texture.Sample(g_sampler, input.texcoord);
    return output;
};
