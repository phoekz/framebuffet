#include "shaders/samplers.hlsli"

//
// Types
//

struct Constants {
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

struct PixelOutput {
    float4 color: SV_TARGET;
};

//
// Bindings
//

struct Bindings {
    uint constants;
    uint texture;
    uint shadow_texture;
};
ConstantBuffer<Bindings> g_bindings: register(b0);

//
// Entry points
//

VertexOutput vs_main(VertexInput input) {
    ConstantBuffer<Constants> constants = ResourceDescriptorHeap[g_bindings.constants];

    VertexOutput output;
    output.position = mul(constants.transform, float4(input.position, 1.0f));
    output.normal = input.normal;
    output.texcoord = input.texcoord;
    output.shadow_coord = mul(constants.light_transform, float4(input.position, 1.0f));
    return output;
}

PixelOutput ps_main(VertexOutput input) {
    ConstantBuffer<Constants> constants = ResourceDescriptorHeap[g_bindings.constants];
    Texture2D texture = ResourceDescriptorHeap[g_bindings.texture];
    Texture2D shadow_texture = ResourceDescriptorHeap[g_bindings.shadow_texture];
    SamplerState samp = SamplerDescriptorHeap[(uint)GpuSamplerType::LinearClamp];
    SamplerComparisonState shadow_samp = SamplerDescriptorHeap[(uint)GpuSamplerType::Shadow];

    float3 shadow_coord = input.shadow_coord.xyz / input.shadow_coord.w;
    float3 shadow_texcoord = shadow_coord.xyz * 0.5f + 0.5f;
    float shadow =
        shadow_texture.SampleCmpLevelZero(shadow_samp, shadow_texcoord.xy, shadow_texcoord.z).r;

    float3 color = texture.Sample(samp, input.texcoord).rgb;
    float n_dot_l = shadow * saturate(dot(input.normal, constants.light_direction));
    float lighting = constants.ambient_light + (1.0 - constants.ambient_light) * n_dot_l;
    PixelOutput output;
    output.color = float4(color * lighting, 1.0f);
    return output;
}
