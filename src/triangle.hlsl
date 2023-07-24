cbuffer ConstantBuffer: register(b0) {
    float4x4 transform;
    float padding[48];
};

struct VertexInput {
    float3 position: POSITION;
    float3 normal: NORMAL;
    float2 texcoord: TEXCOORD;
};

struct VertexOutput {
    float4 position: SV_POSITION;
    float4 color: COLOR;
    float2 texcoord: TEXCOORD;
};

VertexOutput vertex_shader(VertexInput input) {
    VertexOutput output;
    output.position = mul(transform, float4(input.position, 1.0));
    output.color = float4(input.normal * 0.5 + 0.5, 1.0);
    output.texcoord = input.texcoord;
    return output;
}

struct PixelOutput {
    float4 color: SV_TARGET;
};

Texture2D g_texture: register(t0);
SamplerState g_sampler: register(s0);

PixelOutput pixel_shader(VertexOutput input) {
    PixelOutput output;
    output.color = input.color * g_texture.Sample(g_sampler, input.texcoord);
    return output;
}
