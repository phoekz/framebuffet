#include <kitchen/gpu/samplers.hlsli>
#include <kitchen/kcn/core.hlsli>
#include <kitchen/kcn/gui.hlsli>

struct Vertex {
    float2 position;
    float2 texcoord;
    uint8_t4_packed color;
};

struct VertexOutput {
    float4 position : SV_Position;
    float4 color : ATTRIBUTE0;
    float2 texcoord : ATTRIBUTE1;
};

const ConstantBuffer<Bindings> g_bindings: register(b0);

VertexOutput draw_vs(fb::VertexInput input) {
    const ConstantBuffer<Constants> constants = ResourceDescriptorHeap[g_bindings.constants];
    const StructuredBuffer<Vertex> vertices = ResourceDescriptorHeap[g_bindings.vertices];
    const Vertex vertex = vertices[g_bindings.base_vertex + input.vertex_id];

    VertexOutput output;
    output.position = mul(constants.transform, float4(vertex.position, 0.0f, 1.0f));
    output.color = float4(unpack_u8u32(vertex.color)) / 255.0;
    // https://github.com/ocornut/imgui/issues/578
    output.color.xyz = fb::linear_from_srgb(output.color.xyz);
    output.texcoord = vertex.texcoord;
    return output;
}

fb::PixelOutput<1> draw_ps(VertexOutput input) {
    const Texture2D texture = ResourceDescriptorHeap[g_bindings.texture];
    const SamplerState sampler = SamplerDescriptorHeap[(uint)GpuSampler::LinearWrap];

    fb::PixelOutput<1> output;
    output.color = input.color * texture.Sample(sampler, input.texcoord);
    return output;
}
