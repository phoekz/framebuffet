#include <stockcube/techniques/model/model.hlsli>
#include <kitchen/gpu/samplers.hlsli>
#include <kitchen/graphics/core.hlsli>

ConstantBuffer<Bindings> g_bindings: register(b0);

struct VertexOutput {
    float4 position: SV_Position;
    float3 normal: ATTRIBUTE0;
    float3 world_position: ATTRIBUTE1;
};

VertexOutput vs(FbVertexInput input) {
    ConstantBuffer<Constants> constants = ResourceDescriptorHeap[g_bindings.constants];
    StructuredBuffer<FbVertex> vertices = ResourceDescriptorHeap[g_bindings.vertices];
    FbVertex vertex = vertices[input.vertex_id];

    VertexOutput output;
    output.position = mul(constants.transform, float4(vertex.position, 1.0f));
    output.normal = vertex.normal;
    output.world_position = vertex.position;
    return output;
}

FbPixelOutput<1> ps(VertexOutput input) {
    ConstantBuffer<Constants> constants = ResourceDescriptorHeap[g_bindings.constants];
    Texture2D<float2> lut_texture = ResourceDescriptorHeap[g_bindings.lut_texture];
    TextureCube<float3> irr_texture = ResourceDescriptorHeap[g_bindings.irr_texture];
    TextureCube<float3> rad_texture = ResourceDescriptorHeap[g_bindings.rad_texture];
    SamplerState sampler = SamplerDescriptorHeap[(uint)GpuSampler::LinearClamp];

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
    const float3 color = diffuse + specular;

    FbPixelOutput<1> output;
    output.color = float4(color, 1.0f);
    return output;
}
