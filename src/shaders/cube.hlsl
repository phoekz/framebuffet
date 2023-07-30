cbuffer ConstantBuffer: register(b0) {
    float4x4 transform;
    float pad[48];
};

struct VertexInput {
    float3 position: POSITION;
    float3 normal: NORMAL;
    float2 texcoord: TEXCOORD;
};

struct VertexOutput {
    float4 position: SV_POSITION;
    float3 normal: NORMAL;
    float2 texcoord: TEXCOORD;
};

VertexOutput vs_main(VertexInput input) {
    VertexOutput output;
    output.position = mul(transform, float4(input.position, 1.0f));
    output.normal = input.normal;
    output.texcoord = input.texcoord;
    return output;
}

struct PixelOutput {
    float4 color: SV_TARGET;
};

Texture2D g_texture: register(t0);
SamplerState g_sampler: register(s0);

PixelOutput ps_main(VertexOutput input) {
    float3 color = g_texture.Sample(g_sampler, input.texcoord).rgb;
    float3 light = normalize(float3(1.0f, 1.0f, 1.0f));
    float lighting = 0.25 + 0.75 * saturate(dot(input.normal, light));
    PixelOutput output;
    output.color = float4(color * lighting, 1.0f);
    return output;
}
