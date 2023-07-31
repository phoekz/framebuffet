cbuffer ConstantBuffer: register(b0) {
    float4x4 transform;
    float4x4 light_transform;
    float3 light_direction;
    float ambient_light;
    float pad[28];
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
    float4 shadow_coord: TEXCOORD1;
};

VertexOutput vs_main(VertexInput input) {
    VertexOutput output;
    output.position = mul(transform, float4(input.position, 1.0f));
    output.normal = input.normal;
    output.texcoord = input.texcoord;
    output.shadow_coord = mul(light_transform, float4(input.position, 1.0f));
    return output;
}

struct PixelOutput {
    float4 color: SV_TARGET;
};

Texture2D g_texture: register(t1);
Texture2D g_shadow_texture: register(t2);
SamplerState g_sampler: register(s3);
SamplerComparisonState g_shadow_sampler: register(s4);

PixelOutput ps_main(VertexOutput input) {
    float3 shadow_coord = input.shadow_coord.xyz / input.shadow_coord.w;
    float2 shadow_texcoord = shadow_coord.xy * 0.5f + 0.5f;
    float shadow =
        g_shadow_texture.SampleCmpLevelZero(g_shadow_sampler, shadow_texcoord.xy, shadow_coord.z).r;
    // float shadow = shadow_depth < shadow_coord.z ? 0.0f : 1.0f;

    float3 color = g_texture.Sample(g_sampler, input.texcoord).rgb;
    float n_dot_l = shadow * saturate(dot(input.normal, light_direction));
    float lighting = ambient_light + (1.0 - ambient_light) * n_dot_l;
    PixelOutput output;
    output.color = float4(color * lighting, 1.0f);
    return output;
}
