#include <kitchen/gpu/samplers.hlsli>
#include <kitchen/graphics/core.hlsli>
#include <kitchen/graphics/brdf.hlsli>
#include <buffet/demos/crate/crate.hlsli>

struct VertexOutput {
    float4 position: SV_Position;
    float3 normal: ATTRIBUTE0;
    float2 texcoord: ATTRIBUTE1;
    float4 tangent: ATTRIBUTE2;
    float3 bitangent: ATTRIBUTE3;
    float3 world_position: ATTRIBUTE4;
};

ConstantBuffer<Bindings> g_bindings: register(b0);

VertexOutput draw_vs(FbVertexInput input) {
    ConstantBuffer<Constants> constants = ResourceDescriptorHeap[g_bindings.constants];
    StructuredBuffer<FbVertex> vertices = ResourceDescriptorHeap[g_bindings.vertices];
    FbVertex vertex = vertices[input.vertex_id];

    float3 bitangent = vertex.tangent.w * cross(vertex.normal, vertex.tangent.xyz);

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

FbPixelOutput<1> draw_ps(VertexOutput input) {
    ConstantBuffer<Constants> constants = ResourceDescriptorHeap[g_bindings.constants];
    Texture2D<float4> base_color_texture = ResourceDescriptorHeap[g_bindings.base_color_texture];
    Texture2D<float4> normal_texture = ResourceDescriptorHeap[g_bindings.normal_texture];
    Texture2D<float4> metallic_roughness_texture =
        ResourceDescriptorHeap[g_bindings.metallic_roughness_texture];
    SamplerState sampler = SamplerDescriptorHeap[g_bindings.sampler];

    const float3 direction_to_viewer = normalize(constants.camera_position - input.world_position);
    const float3 direction_to_light = constants.light_direction;

    const float3 base_color = base_color_texture.Sample(sampler, input.texcoord).rgb;
    const float3 normal_sample =
        normalize(normal_texture.Sample(sampler, input.texcoord).rgb * 2.0f - 1.0f);
    const float metallic = metallic_roughness_texture.Sample(sampler, input.texcoord).b;
    const float roughness = metallic_roughness_texture.Sample(sampler, input.texcoord).g;

    const float3x3 tbn_basis = float3x3(input.tangent.xyz, input.bitangent, input.normal);
    const float3 shading_normal = mul(normal_sample, tbn_basis);

    MaterialProperties material;
    material.base_color = base_color;
    material.metalness = metallic;
    material.roughness = roughness;
    float3 brdf =
        evaluate_combined_brdf(shading_normal, direction_to_light, direction_to_viewer, material);

    const float ndotl = saturate(dot(shading_normal, direction_to_light));
    const float lighting = compute_lighting(constants, ndotl);

    float3 final_color;
    switch (constants.output_mode) {
        case OutputMode::Shaded: {
            final_color = brdf * lighting;
            break;
        }
        case OutputMode::ShadingNormal: {
            final_color = 0.5f * (shading_normal + 1.0f);
            break;
        }
        case OutputMode::Lighting: {
            final_color = lighting.xxx;
            break;
        }
        case OutputMode::VertexLighting: {
            const float vertex_ndotl = saturate(dot(input.normal, direction_to_light));
            const float vertex_lighting = compute_lighting(constants, vertex_ndotl);
            final_color = vertex_lighting.xxx;
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

    FbPixelOutput<1> output;
    output.color = float4(final_color, 1.0f);
    return output;
}
