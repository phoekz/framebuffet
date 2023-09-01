#include <kitchen/gpu/samplers.hlsli>
#include <kitchen/graphics/core.hlsli>
#include <kitchen/graphics/brdf.hlsli>
#include <buffet/demos/env/env.hlsli>

//
// Background.
//

struct BackgroundVertexOutput {
    float4 position: SV_Position;
    float3 normal: ATTRIBUTE0;
    float2 texcoord: ATTRIBUTE1;
    float3 direction: ATTRIBUTE2;
};

ConstantBuffer<BackgroundBindings> g_background_bindings: register(b0);

BackgroundVertexOutput background_vs(FbVertexInput input) {
    ConstantBuffer<BackgroundConstants> constants =
        ResourceDescriptorHeap[g_background_bindings.constants];
    StructuredBuffer<FbVertex> vertices = ResourceDescriptorHeap[g_background_bindings.vertices];
    FbVertex vertex = vertices[input.vertex_id];
    float3 direction = vertex.position;

    BackgroundVertexOutput output;
    output.position = mul(constants.transform, float4(vertex.position, 1.0f));
    output.position = output.position.xyww; // Forces far plane.
    output.normal = vertex.normal;
    output.texcoord = vertex.texcoord;
    output.direction = direction;
    return output;
}

FbPixelOutput<1> background_ps(BackgroundVertexOutput input) {
    ConstantBuffer<BackgroundConstants> constants =
        ResourceDescriptorHeap[g_background_bindings.constants];
    TextureCube<float4> texture = ResourceDescriptorHeap[g_background_bindings.texture];
    SamplerState samp = SamplerDescriptorHeap[(uint)GpuSampler::LinearWrap];

    const float roughness = constants.roughness;
    const uint mip_count = constants.mip_count;
    const float mip_level = roughness * float(mip_count - 1);
    float3 color = texture.SampleLevel(samp, input.direction, mip_level).rgb;

    if (constants.tonemap) {
        color = fb_tonemap_aces(color);
    }

    color *= constants.exposure;

    FbPixelOutput<1> output;
    output.color = float4(color, 1.0f);
    return output;
}

//
// Model.
//

struct ModelVertexOutput {
    float4 position: SV_Position;
    float3 normal: ATTRIBUTE0;
    float3 world_position: ATTRIBUTE1;
};

ConstantBuffer<ModelBindings> g_model_bindings: register(b0);

ModelVertexOutput model_vs(FbVertexInput input) {
    ConstantBuffer<ModelConstants> constants = ResourceDescriptorHeap[g_model_bindings.constants];
    StructuredBuffer<FbVertex> vertices = ResourceDescriptorHeap[g_model_bindings.vertices];
    FbVertex vertex = vertices[input.vertex_id];

    ModelVertexOutput output;
    output.position = mul(constants.transform, float4(vertex.position, 1.0f));
    output.normal = vertex.normal;
    output.world_position = vertex.position;
    return output;
}

FbPixelOutput<1> model_ps(ModelVertexOutput input) {
    ConstantBuffer<ModelConstants> constants = ResourceDescriptorHeap[g_model_bindings.constants];
    Texture2D<float2> lut_texture = ResourceDescriptorHeap[g_model_bindings.lut_texture];
    TextureCube<float3> irr_texture = ResourceDescriptorHeap[g_model_bindings.irr_texture];
    TextureCube<float3> rad_texture = ResourceDescriptorHeap[g_model_bindings.rad_texture];
    SamplerState samp = SamplerDescriptorHeap[(uint)GpuSampler::LinearClamp];

    const float3 n = normalize(input.normal);
    const float3 v = normalize(constants.camera_position - input.world_position);
    const float3 r = reflect(-v, n);
    const float ndotv = saturate(dot(n, v));
    const float roughness = constants.roughness;
    const float mip_count = constants.mip_count;
    const float radiance_lod = roughness * float(mip_count - 1);

    const float2 f0_scale_bias = lut_texture.Sample(samp, float2(ndotv, roughness));
    const float f0_scale = f0_scale_bias.x;
    const float f0_bias = f0_scale_bias.y;
    const float3 irradiance = irr_texture.SampleLevel(samp, n, 0);
    const float3 radiance = rad_texture.SampleLevel(samp, r, radiance_lod);

    const float3 base_color = float3(1.0f, 1.0f, 1.0f);
    const float3 specular_f0 = lerp(0.04f.xxx, base_color, constants.metallic);

    const float3 diffuse = irradiance * base_color;
    const float3 specular = radiance * (specular_f0 * f0_scale + f0_bias);
    float3 color = diffuse + specular;

    if (constants.tonemap) {
        color = fb_tonemap_aces(color);
    }

    FbPixelOutput<1> output;
    output.color = float4(color, 1.0f);
    return output;
}
