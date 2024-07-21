#include <stockcube/techniques/screen/screen.hlsli>
#include <kitchen/gpu/samplers.hlsli>
#include <kitchen/graphics/core.hlsli>

ConstantBuffer<Bindings> g_bindings: register(b0);

struct VertexOutput {
    float4 position: SV_Position;
    float2 texcoord: ATTRIBUTE0;
    nointerpolation uint texture: ATTRIBUTE1;
    nointerpolation uint texture_face_id: ATTRIBUTE2;
    nointerpolation uint texture_mip_id: ATTRIBUTE3;
    nointerpolation uint sampler: ATTRIBUTE4;
};

VertexOutput vs(fb::VertexInput input) {
    ConstantBuffer<Constants> constants = ResourceDescriptorHeap[g_bindings.constants];
    StructuredBuffer<Vertex> vertices = ResourceDescriptorHeap[g_bindings.vertices];
    StructuredBuffer<Instance> instances = ResourceDescriptorHeap[g_bindings.instances];
    const Vertex vertex = vertices[input.vertex_id];
    const Instance instance = instances[input.instance_id];

    const float2 position = vertex.position * instance.scale + instance.offset;

    VertexOutput output;
    output.position = mul(constants.transform, float4(position, 0.0f, 1.0f));
    output.texcoord = vertex.texcoord;
    output.texture = instance.texture;
    output.texture_face_id = instance.texture_face_id;
    output.texture_mip_id = instance.texture_mip_id;
    output.sampler = instance.sampler;
    return output;
}

fb::PixelOutput<1> ps(VertexOutput input) {
    Texture2DArray<float3> texture = ResourceDescriptorHeap[input.texture];
    SamplerState texture_sampler = SamplerDescriptorHeap[input.sampler];

    const float3 color = texture.SampleLevel(
        texture_sampler,
        float3(input.texcoord, input.texture_face_id),
        input.texture_mip_id
    );

    fb::PixelOutput<1> output;
    output.color = float4(color, 1.0f);
    return output;
}
