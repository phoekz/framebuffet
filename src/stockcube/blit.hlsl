#include <stockcube/blit.hlsli>
#include <kitchen/gpu/samplers.hlsli>
#include <kitchen/graphics/core.hlsli>

struct VertexOutput {
    float4 position: SV_Position;
    float2 texcoord: ATTRIBUTE0;
};

ConstantBuffer<Bindings> g_bindings: register(b0);

VertexOutput draw_vs(FbVertexInput input) {
    VertexOutput output;
    fb_fullscreen_triangle(input.vertex_id, output.position, output.texcoord);
    return output;
}

FbPixelOutput1 draw_ps(VertexOutput input) {
    Texture2D texture = ResourceDescriptorHeap[g_bindings.texture];
    SamplerState texture_sampler = SamplerDescriptorHeap[(uint)GpuSampler::LinearClamp];
    float3 color = texture.Sample(texture_sampler, input.texcoord).rgb;
    FbPixelOutput1 output;
    output.color = float4(color, 1.0f);
    return output;
}
