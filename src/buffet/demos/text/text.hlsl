#include <buffet/demos/text/text.hlsli>
#include <kitchen/gpu/samplers.hlsli>
#include <kitchen/graphics/core.hlsli>

ConstantBuffer<BackgroundBindings> g_background_bindings: register(b0);
ConstantBuffer<GlyphBindings> g_glyph_bindings: register(b0);

//
// Background.
//

struct BackgroundVertexOutput {
    float4 position: SV_Position;
    float3 normal: ATTRIBUTE0;
    float2 texcoord: ATTRIBUTE1;
    float3 direction: ATTRIBUTE2;
};

BackgroundVertexOutput background_vs(FbVertexInput input) {
    ConstantBuffer<BackgroundConstants> constants =
        ResourceDescriptorHeap[g_background_bindings.constants];
    StructuredBuffer<FbVertex> vertices = ResourceDescriptorHeap[g_background_bindings.vertices];
    FbVertex vertex = vertices[input.vertex_id];
    float3 direction = vertex.position;

    BackgroundVertexOutput output;
    output.position = mul(constants.transform, float4(vertex.position, 1.0f));
    output.position = output.position.xyww; // Forces far plane.
    output.normal = vertex.normal;
    output.texcoord = vertex.texcoord;
    output.direction = direction;
    return output;
}

FbPixelOutput<1> background_ps(BackgroundVertexOutput input) {
    TextureCube<float3> irr_texture = ResourceDescriptorHeap[g_background_bindings.irr_texture];
    SamplerState sampler = SamplerDescriptorHeap[g_background_bindings.sampler];

    const float3 direction = normalize(input.direction);
    const float3 irradiance = irr_texture.SampleLevel(sampler, direction, 0);

    FbPixelOutput<1> output;
    output.color = float4(irradiance, 1.0f);
    return output;
}

//
// Glyph.
//

struct GlyphVertexOutput {
    float4 position: SV_Position;
    float3 direction: ATTRIBUTE0;
};

GlyphVertexOutput glyph_vs(FbVertexInput input) {
    ConstantBuffer<GlyphConstants> constants = ResourceDescriptorHeap[g_glyph_bindings.constants];
    StructuredBuffer<FbVertex> vertices = ResourceDescriptorHeap[g_glyph_bindings.vertices];
    StructuredBuffer<GlyphInstance> instances = ResourceDescriptorHeap[g_glyph_bindings.instances];
    FbVertex vertex = vertices[input.vertex_id + g_glyph_bindings.base_vertex];
    GlyphInstance instance = instances[g_glyph_bindings.instance_id];

    const float3 position = vertex.position + instance.position;
    const float3 direction = mul((float3x3)constants.scene_transform, vertex.normal);

    GlyphVertexOutput output;
    output.position = mul(constants.transform, float4(position, 1.0f));
    output.direction = direction;
    return output;
}

FbPixelOutput<1> glyph_ps(GlyphVertexOutput input) {
    ConstantBuffer<GlyphConstants> constants = ResourceDescriptorHeap[g_glyph_bindings.constants];
    SamplerState sampler = SamplerDescriptorHeap[g_glyph_bindings.sampler];
    TextureCube<float3> irr_texture = ResourceDescriptorHeap[g_glyph_bindings.irr_texture];

    const float3 direction = normalize(input.direction);
    const float3 irradiance = constants.color.rgb * irr_texture.SampleLevel(sampler, direction, 0);

    FbPixelOutput<1> output;
    output.color = float4(irradiance, 1.0f);
    return output;
}
