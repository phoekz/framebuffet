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
    Texture2D texture = ResourceDescriptorHeap[g_bindings.texture];
    SamplerState linear_clamp = SamplerDescriptorHeap[(uint)GpuSamplerType::LinearClamp];

    const float3 base_color = texture.Sample(linear_clamp, input.texcoord).rgb;
    const float n_dot_l = saturate(dot(input.normal, constants.light_direction));
    const float lighting = constants.light_ambient + n_dot_l * (1.0f - constants.light_ambient);

    float3 final_color;
    switch (constants.output_mode) {
        case OutputMode::Shaded: {
            final_color = base_color * lighting;
            break;
        }
        case OutputMode::Lighting: {
            final_color = lighting.xxx;
            break;
        }
        case OutputMode::BaseColor: {
            final_color = base_color;
            break;
        }
        case OutputMode::TexCoord: {
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
