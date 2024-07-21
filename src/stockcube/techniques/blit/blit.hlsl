#include <stockcube/techniques/blit/blit.hlsli>
#include <kitchen/gpu/samplers.hlsli>
#include <kitchen/graphics/core.hlsli>

ConstantBuffer<Bindings> g_bindings: register(b0);

struct VertexOutput {
    float4 position: SV_Position;
    float2 texcoord: ATTRIBUTE0;
};

VertexOutput vs(fb::VertexInput input) {
    VertexOutput output;
    fb::fullscreen_triangle(input.vertex_id, output.position, output.texcoord);
    return output;
}

fb::PixelOutput<1> ps(VertexOutput input) {
    ConstantBuffer<Constants> constants = ResourceDescriptorHeap[g_bindings.constants];
    Texture2D texture = ResourceDescriptorHeap[g_bindings.texture];
    SamplerState texture_sampler = SamplerDescriptorHeap[(uint)GpuSampler::LinearClamp];

    float3 color = texture.Sample(texture_sampler, input.texcoord).rgb;
    if (constants.tonemap) {
        color = fb::tonemap_aces(color);
    }
    color *= constants.exposure;

    fb::PixelOutput<1> output;
    output.color = float4(color, 1.0f);
    return output;
}
