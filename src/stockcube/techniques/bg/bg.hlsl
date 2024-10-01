#include <stockcube/techniques/bg/bg.hlsli>
#include <kitchen/gpu/samplers.hlsli>
#include <kitchen/kcn/core.hlsli>

ConstantBuffer<Bindings> g_bindings: register(b0);

struct VertexOutput {
    float4 position: SV_Position;
    float3 normal: ATTRIBUTE0;
    float2 texcoord: ATTRIBUTE1;
    float3 direction: ATTRIBUTE2;
};

VertexOutput vs(fb::VertexInput input) {
    const ConstantBuffer<Constants> constants = ResourceDescriptorHeap[g_bindings.constants];
    const StructuredBuffer<fb::Vertex> vertices = ResourceDescriptorHeap[g_bindings.vertices];
    const fb::Vertex vertex = vertices[input.vertex_id];
    const float3 direction = vertex.position;

    VertexOutput output;
    output.position = mul(constants.transform, float4(vertex.position, 1.0f));
    output.position = output.position.xyww; // Forces far plane.
    output.normal = vertex.normal;
    output.texcoord = vertex.texcoord;
    output.direction = direction;
    return output;
}

fb::PixelOutput<1> ps(VertexOutput input) {
    const ConstantBuffer<Constants> constants = ResourceDescriptorHeap[g_bindings.constants];
    const TextureCube<float4> texture = ResourceDescriptorHeap[g_bindings.texture];
    const SamplerState texture_sampler = SamplerDescriptorHeap[g_bindings.sampler];

    const float roughness = constants.roughness;
    const uint mip_count = constants.mip_count;
    const float mip_level = roughness * float(mip_count - 1);

    const float3 color = texture.SampleLevel(texture_sampler, input.direction, mip_level).rgb;

    fb::PixelOutput<1> output;
    output.color = float4(color, 1.0f);
    return output;
}
