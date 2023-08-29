#include <stockcube/screen.hlsli>
#include <kitchen/gpu/samplers.hlsli>
#include <kitchen/graphics/core.hlsli>

struct VertexOutput {
    float4 position: SV_Position;
    float2 texcoord: ATTRIBUTE0;
    nointerpolation uint texture: ATTRIBUTE1;
};

ConstantBuffer<Bindings> g_bindings: register(b0);

VertexOutput draw_vs(FbVertexInput input) {
    ConstantBuffer<Constants> constants = ResourceDescriptorHeap[g_bindings.constants];
    StructuredBuffer<Vertex> vertices = ResourceDescriptorHeap[g_bindings.vertices];
    StructuredBuffer<Instance> instances = ResourceDescriptorHeap[g_bindings.instances];
    Vertex vertex = vertices[input.vertex_id];
    Instance instance = instances[input.instance_id];

    const float2 position = vertex.position * instance.scale + instance.offset;

    VertexOutput output;
    output.position = mul(constants.transform, float4(position, 0.0f, 1.0f));
    output.texcoord = vertex.texcoord;
    output.texture = instance.texture;
    return output;
}

FbPixelOutput1 draw_ps(VertexOutput input) {
    Texture2D texture = ResourceDescriptorHeap[input.texture];
    SamplerState texture_sampler = SamplerDescriptorHeap[(uint)GpuSampler::PointClamp];

    FbPixelOutput1 output;
    output.color = texture.Sample(texture_sampler, input.texcoord);
    return output;
}
