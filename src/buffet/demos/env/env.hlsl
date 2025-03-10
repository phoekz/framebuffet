#include <buffet/demos/env/env.hlsli>
#include <kitchen/gpu/samplers.hlsli>
#include <kitchen/kcn/core.hlsli>

//
// Background.
//

struct BackgroundVertexOutput {
    float4 position: SV_Position;
    float3 normal: ATTRIBUTE0;
    float2 texcoord: ATTRIBUTE1;
    float3 direction: ATTRIBUTE2;
};

const ConstantBuffer<BackgroundBindings> g_background_bindings: register(b0);

BackgroundVertexOutput background_vs(fb::VertexInput input) {
    const ConstantBuffer<BackgroundConstants> constants =
        ResourceDescriptorHeap[g_background_bindings.constants];
    const StructuredBuffer<fb::Vertex> vertices =
        ResourceDescriptorHeap[g_background_bindings.vertices];
    const fb::Vertex vertex = vertices[input.vertex_id];
    const float3 direction = vertex.position;

    BackgroundVertexOutput output;
    output.position = mul(constants.transform, float4(vertex.position, 1.0f));
    output.position = output.position.xyww; // Forces far plane.
    output.normal = vertex.normal;
    output.texcoord = vertex.texcoord;
    output.direction = direction;
    return output;
}

fb::PixelOutput<1> background_ps(BackgroundVertexOutput input) {
    const ConstantBuffer<BackgroundConstants> constants =
        ResourceDescriptorHeap[g_background_bindings.constants];
    const TextureCube<float4> texture = ResourceDescriptorHeap[g_background_bindings.texture];
    const SamplerState sampler = SamplerDescriptorHeap[(uint)GpuSampler::LinearWrap];
    const float roughness = constants.roughness;
    const uint mip_count = constants.mip_count;
    const float mip_level = roughness * float(mip_count - 1);

    float3 color = texture.SampleLevel(sampler, input.direction, mip_level).rgb;
    if (constants.tonemap) {
        color = fb::tonemap_aces(color);
    }
    color *= constants.exposure;

    fb::PixelOutput<1> output;
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

ModelVertexOutput model_vs(fb::VertexInput input) {
    const ConstantBuffer<ModelConstants> constants =
        ResourceDescriptorHeap[g_model_bindings.constants];
    const StructuredBuffer<fb::Vertex> vertices = ResourceDescriptorHeap[g_model_bindings.vertices];
    const fb::Vertex vertex = vertices[input.vertex_id];

    ModelVertexOutput output;
    output.position = mul(constants.transform, float4(vertex.position, 1.0f));
    output.normal = vertex.normal;
    output.world_position = vertex.position;
    return output;
}

fb::PixelOutput<1> model_ps(ModelVertexOutput input) {
    const ConstantBuffer<ModelConstants> constants =
        ResourceDescriptorHeap[g_model_bindings.constants];
    const Texture2D<float2> lut_texture = ResourceDescriptorHeap[g_model_bindings.lut_texture];
    const TextureCube<float3> irr_texture = ResourceDescriptorHeap[g_model_bindings.irr_texture];
    const TextureCube<float3> rad_texture = ResourceDescriptorHeap[g_model_bindings.rad_texture];
    const SamplerState sampler = SamplerDescriptorHeap[(uint)GpuSampler::LinearClamp];

    const float3 n = normalize(input.normal);
    const float3 v = normalize(constants.camera_position - input.world_position);
    const float3 r = reflect(-v, n);
    const float ndotv = saturate(dot(n, v));
    const float roughness = constants.roughness;
    const float mip_count = constants.mip_count;
    const float radiance_lod = roughness * float(mip_count - 1);

    const float2 f0_scale_bias = lut_texture.Sample(sampler, float2(ndotv, roughness));
    const float f0_scale = f0_scale_bias.x;
    const float f0_bias = f0_scale_bias.y;
    const float3 irradiance = irr_texture.SampleLevel(sampler, n, 0);
    const float3 radiance = rad_texture.SampleLevel(sampler, r, radiance_lod);

    const float3 base_color = float3(1.0f, 1.0f, 1.0f);
    const float3 specular_f0 = lerp(0.04f.xxx, base_color, constants.metallic);

    const float3 diffuse = irradiance * base_color;
    const float3 specular = radiance * (specular_f0 * f0_scale + f0_bias);

    float3 color = diffuse + specular;
    if (constants.tonemap) {
        color = fb::tonemap_aces(color);
    }

    fb::PixelOutput<1> output;
    output.color = float4(color, 1.0f);
    return output;
}
