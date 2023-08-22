#include <gpu/samplers.hlsli>
#include <demos/core.hlsli>
#include <demos/cube/cube.hlsli>

struct VertexOutput {
    float4 position: SV_Position;
    float3 normal: ATTRIBUTE0;
    float2 texcoord: ATTRIBUTE1;
    float4 tangent: ATTRIBUTE2;
    float3 bitangent: ATTRIBUTE3;
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
    return output;
}

FbPixelOutput1 draw_ps(VertexOutput input) {
    ConstantBuffer<Constants> constants = ResourceDescriptorHeap[g_bindings.constants];
    Texture2D<float4> base_color_texture = ResourceDescriptorHeap[g_bindings.base_color_texture];
    Texture2D<float4> normal_texture = ResourceDescriptorHeap[g_bindings.normal_texture];
    Texture2D<float4> metallic_roughness_texture =
        ResourceDescriptorHeap[g_bindings.metallic_roughness_texture];
    SamplerState linear_clamp = SamplerDescriptorHeap[(uint)GpuSamplerType::LinearClamp];

    const float3 base_color = base_color_texture.Sample(linear_clamp, input.texcoord).rgb;
    const float3 normal_sample =
        normalize(normal_texture.Sample(linear_clamp, input.texcoord).rgb * 2.0f - 1.0f);
    const float metallic = metallic_roughness_texture.Sample(linear_clamp, input.texcoord).b;
    const float roughness = metallic_roughness_texture.Sample(linear_clamp, input.texcoord).g;

    const float3x3 tbn_basis = float3x3(input.tangent.xyz, input.bitangent, input.normal);
    const float3 shading_normal = mul(normal_sample, tbn_basis);

    const float ndotl = saturate(dot(shading_normal, constants.light_direction));
    const float lighting = constants.light_ambient + ndotl * (1.0f - constants.light_ambient);

    float3 final_color;
    switch (constants.output_mode) {
        case OutputMode::Shaded: {
            final_color = base_color * lighting;
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
            const float vertex_ndotl = saturate(dot(input.normal, constants.light_direction));
            const float vertex_lighting =
                constants.light_ambient + vertex_ndotl * (1.0f - constants.light_ambient);
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
            final_color = float3(input.texcoord, 0.0f);
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
    }

    FbPixelOutput1 output;
    output.color = float4(final_color, 1.0f);
    return output;
}
