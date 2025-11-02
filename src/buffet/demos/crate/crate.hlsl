#include <buffet/demos/crate/crate.hlsli>
#include <kitchen/gpu/samplers.hlsli>
#include <kitchen/kcn/core.hlsli>
#include <kitchen/kcn/brdf.hlsli>

const ConstantBuffer<Bindings> g_bindings: register(b0);

struct VertexOutput {
    float4 position : SV_Position;
    float3 normal : ATTRIBUTE0;
    float2 texcoord : ATTRIBUTE1;
    float4 tangent : ATTRIBUTE2;
    float3 bitangent : ATTRIBUTE3;
    float3 world_position : ATTRIBUTE4;
};

VertexOutput draw_vs(fb::VertexInput input) {
    const ConstantBuffer<Constants> constants = ResourceDescriptorHeap[g_bindings.constants];
    const StructuredBuffer<fb::Vertex> vertices = ResourceDescriptorHeap[g_bindings.vertices];
    const fb::Vertex vertex = vertices[input.vertex_id];

    const float3 bitangent = vertex.tangent.w * cross(vertex.normal, vertex.tangent.xyz);

    VertexOutput output;
    output.position = mul(constants.transform, float4(vertex.position, 1.0f));
    output.normal = vertex.normal;
    output.texcoord = vertex.texcoord;
    output.tangent = vertex.tangent;
    output.bitangent = bitangent;
    output.world_position = vertex.position;
    return output;
}

float compute_lighting(Constants constants, float ndotl) {
    float lighting = constants.light_ambient + ndotl * (1.0f - constants.light_ambient);
    lighting *= constants.light_intensity;
    return lighting;
}

fb::PixelOutput<1> draw_ps(VertexOutput input) {
    // Global resources.
    const ConstantBuffer<Constants> constants = ResourceDescriptorHeap[g_bindings.constants];
    const Texture2D<float4> base_color_texture =
        ResourceDescriptorHeap[g_bindings.base_color_texture];
    const Texture2D<float4> normal_texture = ResourceDescriptorHeap[g_bindings.normal_texture];
    const Texture2D<float4> metallic_roughness_texture =
        ResourceDescriptorHeap[g_bindings.metallic_roughness_texture];
    const SamplerState sampler = SamplerDescriptorHeap[g_bindings.sampler];
    const Texture2D<float2> lut_texture = ResourceDescriptorHeap[g_bindings.lut_texture];
    const TextureCube<float3> irr_texture = ResourceDescriptorHeap[g_bindings.irr_texture];
    const TextureCube<float3> rad_texture = ResourceDescriptorHeap[g_bindings.rad_texture];

    // Materials.
    const float3 base_color = base_color_texture.Sample(sampler, input.texcoord).rgb;
    const float3 normal_sample =
        normalize(normal_texture.Sample(sampler, input.texcoord).rgb * 2.0f - 1.0f);
    const float metallic = metallic_roughness_texture.Sample(sampler, input.texcoord).b;
    const float roughness = metallic_roughness_texture.Sample(sampler, input.texcoord).g;
    const float3 specular_f0 = lerp(0.04f.xxx, base_color, metallic);

    // Normal mapping.
    const float3x3 tbn_basis = float3x3(input.tangent.xyz, input.bitangent, input.normal);
    const float3 shading_normal = mul(normal_sample, tbn_basis);

    // Directions.
    const float3 direction_to_viewer = normalize(constants.camera_position - input.world_position);
    const float3 direction_to_light = constants.light_direction;

    // Evaluate BRDF.
    MaterialProperties material;
    material.base_color = base_color;
    material.metalness = metallic;
    material.roughness = roughness;
    const float3 brdf =
        evaluate_combined_brdf(shading_normal, direction_to_light, direction_to_viewer, material);

    // Static lighting.
    const float3 reflection = reflect(-direction_to_viewer, shading_normal);
    const float ndotv = saturate(dot(shading_normal, reflection));
    const uint mip_count = constants.rad_texture_mip_count;
    const float radiance_lod = roughness * float(mip_count - 1);
    const float2 f0_scale_bias = lut_texture.Sample(sampler, float2(ndotv, roughness));
    const float f0_scale = f0_scale_bias.x;
    const float f0_bias = f0_scale_bias.y;
    const float3 irradiance = irr_texture.SampleLevel(sampler, shading_normal, 0);
    const float3 radiance = rad_texture.SampleLevel(sampler, reflection, radiance_lod);
    const float3 diffuse = irradiance * base_color;
    const float3 specular = radiance * (specular_f0 * f0_scale + f0_bias);

    // Dynamic lighting.
    const float ndotl = saturate(dot(shading_normal, direction_to_light));
    const float lighting = compute_lighting(constants, ndotl);

    float3 final_color;
    switch (constants.output_mode) {
        case OutputMode::Shaded: {
            final_color = diffuse + specular;
            break;
        }
        case OutputMode::EnvDiffuse: {
            final_color = diffuse;
            break;
        }
        case OutputMode::EnvSpecular: {
            final_color = specular;
            break;
        }
        case OutputMode::EnvLut: {
            final_color = float3(f0_scale_bias, 0.0f);
            break;
        }
        case OutputMode::EnvIrradiance: {
            final_color = irradiance;
            break;
        }
        case OutputMode::EnvRadiance: {
            final_color = radiance;
            break;
        }
        case OutputMode::DirectLighting: {
            final_color = brdf * lighting;
            break;
        }
        case OutputMode::DirectBrdf: {
            final_color = brdf;
            break;
        }
        case OutputMode::VertexLighting: {
            const float vertex_ndotl = saturate(dot(input.normal, direction_to_light));
            const float vertex_lighting = compute_lighting(constants, vertex_ndotl);
            final_color = vertex_lighting.xxx;
            break;
        }
        case OutputMode::ShadingNormal: {
            final_color = 0.5f * (shading_normal + 1.0f);
            break;
        }
        case OutputMode::BaseColorTexture: {
            final_color = base_color;
            break;
        }
        case OutputMode::NormalTexture: {
            final_color = 0.5f * (normal_sample + 1.0f);
            break;
        }
        case OutputMode::Metallic: {
            final_color = metallic.xxx;
            break;
        }
        case OutputMode::Roughness: {
            final_color = roughness.xxx;
            break;
        }
        case OutputMode::VertexTexCoord: {
            final_color = float3(frac(input.texcoord), 0.0f);
            break;
        }
        case OutputMode::VertexNormal: {
            final_color = 0.5f * (input.normal + 1.0f);
            break;
        }
        case OutputMode::VertexTangent: {
            final_color = 0.5f * (input.tangent.xyz + 1.0f);
            break;
        }
        case OutputMode::VertexBitangent: {
            final_color = 0.5f * (input.bitangent + 1.0f);
            break;
        }
        case OutputMode::LevelOfDetail: {
            float lod = base_color_texture.CalculateLevelOfDetail(sampler, input.texcoord);
            lod = frac(lod);
            final_color = lod.xxx;
            break;
        }
        case OutputMode::DirectionToViewer: {
            final_color = 0.5f * (direction_to_viewer + 1.0f);
            break;
        }
        case OutputMode::DirectionToLight: {
            final_color = 0.5f * (direction_to_light + 1.0f);
            break;
        }
    }

    fb::PixelOutput<1> output;
    output.color = float4(final_color, 1.0f);
    return output;
}
