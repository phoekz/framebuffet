// This implementation is adapted from
// https://www.iryoku.com/next-generation-post-processing-in-call-of-duty-advanced-warfare/

#include <buffet/demos/saber/saber.hlsli>
#include <kitchen/gpu/samplers.hlsli>
#include <kitchen/kcn/core.hlsli>

//
// Scene pass.
//

const ConstantBuffer<SceneBindings> g_scene_bindings: register(b0);

struct SceneVertexOutput {
    float4 position: SV_Position;
};

SceneVertexOutput scene_vs(fb::VertexInput input) {
    const ConstantBuffer<Constants> constants = ResourceDescriptorHeap[g_scene_bindings.constants];
    const StructuredBuffer<fb::Vertex> vertices = ResourceDescriptorHeap[g_scene_bindings.vertices];
    const StructuredBuffer<SceneInstance> instances =
        ResourceDescriptorHeap[g_scene_bindings.instances];
    const fb::Vertex vertex = vertices[input.vertex_id];
    const float4x4 transform = instances[input.instance_id].transform;

    SceneVertexOutput output;
    output.position = mul(constants.transform, mul(transform, float4(vertex.position, 1.0f)));
    return output;
}

fb::PixelOutput<1> scene_ps(SceneVertexOutput /*input*/) {
    const ConstantBuffer<Constants> constants = ResourceDescriptorHeap[g_scene_bindings.constants];

    const float3 saber_color = constants.saber_color_and_intensity.rgb;
    const float saber_intensity = constants.saber_color_and_intensity.a;

    fb::PixelOutput<1> output;
    output.color = float4((saber_color + THRESHOLD) + (saber_color * saber_intensity), 1.0f);
    return output;
}

//
// Threshold pass.
//

ConstantBuffer<ThresholdBindings> g_threshold_bindings: register(b0);

FB_ATTRIBUTE(numthreads, DISPATCH_X, DISPATCH_Y, DISPATCH_Z)
void threshold_cs(fb::ComputeInput input) {
    const uint2 dst_id = input.dispatch_thread_id.xy;
    if (dst_id.x >= g_threshold_bindings.texture_width
        || dst_id.y >= g_threshold_bindings.texture_height) {
        return;
    }

    const uint scene_texture_id = g_threshold_bindings.scene_texture;
    const uint downsample_texture_id = g_threshold_bindings.downsample_texture;
    const float texture_width = float(g_threshold_bindings.texture_width);
    const float texture_height = float(g_threshold_bindings.texture_height);
    const Texture2D<float4> scene_texture = ResourceDescriptorHeap[scene_texture_id];
    const RWTexture2D<float4> downsample_texture = ResourceDescriptorHeap[downsample_texture_id];
    const SamplerState sampler = SamplerDescriptorHeap[(uint)GpuSampler::LinearClamp];

    const float2 dst_id_uv = 0.5f + float2(dst_id);
    const float2 dst_texel_size = 1.0f / float2(texture_width, texture_height);
    const float2 dst_uv = float2(dst_id_uv.x * dst_texel_size.x, dst_id_uv.y * dst_texel_size.y);

    const float3 scene_color = scene_texture.Sample(sampler, dst_uv).rgb;
    const float3 threshold_color = max(0.0f, scene_color - THRESHOLD);

    downsample_texture[dst_id] = float4(threshold_color, 1.0f);
}

//
// Downsample pass.
//

ConstantBuffer<DownsampleBindings> g_downsample_bindings: register(b0);

FB_ATTRIBUTE(numthreads, DISPATCH_X, DISPATCH_Y, DISPATCH_Z)
void downsample_cs(fb::ComputeInput input) {
    const uint2 dst_id = input.dispatch_thread_id.xy;
    if (dst_id.x >= g_downsample_bindings.dst_texture_width
        || dst_id.y >= g_downsample_bindings.dst_texture_height) {
        return;
    }

    const uint src_texture_id = g_downsample_bindings.src_texture;
    const uint src_texture_level = g_downsample_bindings.src_texture_level;
    const uint dst_texture_id = g_downsample_bindings.dst_texture;
    const uint dst_texture_level = g_downsample_bindings.dst_texture_level;
    const uint dst_texture_width = g_downsample_bindings.dst_texture_width;
    const uint dst_texture_height = g_downsample_bindings.dst_texture_height;
    const Texture2D<float4> src_texture = ResourceDescriptorHeap[src_texture_id];
    const RWTexture2D<float4> dst_texture =
        ResourceDescriptorHeap[dst_texture_id + dst_texture_level];
    const SamplerState sampler = SamplerDescriptorHeap[(uint)GpuSampler::LinearClamp];

    const float2 dst_id_uv = 0.5f + float2(dst_id);
    const float2 dst_texel_size = 1.0f / float2(dst_texture_width, dst_texture_height);
    const float2 dst_uv = float2(dst_id_uv.x * dst_texel_size.x, dst_id_uv.y * dst_texel_size.y);

    // Sampling pattern:
    //
    //  a . b . c
    //  . d . e .
    //  f . g . h
    //  . i . j .
    //  k . l . m
    //

    // clang-format off
    const float3 src_a = src_texture.SampleLevel(sampler, dst_uv + dst_texel_size * float2(-1.0f, -1.0f), src_texture_level).rgb;
    const float3 src_b = src_texture.SampleLevel(sampler, dst_uv + dst_texel_size * float2(0.0f, -1.0f), src_texture_level).rgb;
    const float3 src_c = src_texture.SampleLevel(sampler, dst_uv + dst_texel_size * float2(1.0f, -1.0f), src_texture_level).rgb;
    const float3 src_d = src_texture.SampleLevel(sampler, dst_uv + dst_texel_size * float2(-0.5f, -0.5f), src_texture_level).rgb;
    const float3 src_e = src_texture.SampleLevel(sampler, dst_uv + dst_texel_size * float2(0.5f, -0.5f), src_texture_level).rgb;
    const float3 src_f = src_texture.SampleLevel(sampler, dst_uv + dst_texel_size * float2(-1.0f, 0.0f), src_texture_level).rgb;
    const float3 src_g = src_texture.SampleLevel(sampler, dst_uv, src_texture_level).rgb;
    const float3 src_h = src_texture.SampleLevel(sampler, dst_uv + dst_texel_size * float2(1.0f, 0.0f), src_texture_level).rgb;
    const float3 src_i = src_texture.SampleLevel(sampler, dst_uv + dst_texel_size * float2(-0.5f, 0.5f), src_texture_level).rgb;
    const float3 src_j = src_texture.SampleLevel(sampler, dst_uv + dst_texel_size * float2(0.5f, 0.5f), src_texture_level).rgb;
    const float3 src_k = src_texture.SampleLevel(sampler, dst_uv + dst_texel_size * float2(-1.0f, 1.0f), src_texture_level).rgb;
    const float3 src_l = src_texture.SampleLevel(sampler, dst_uv + dst_texel_size * float2(0.0f, 1.0f), src_texture_level).rgb;
    const float3 src_m = src_texture.SampleLevel(sampler, dst_uv + dst_texel_size * float2(1.0f, 1.0f), src_texture_level).rgb;
    // clang-format on

    const float div_0 = 0.5f / 4.0f;
    const float div_1 = 0.125f / 4.0f;
    float3 dst_color = float3(0.0f, 0.0f, 0.0f);
    dst_color += (src_d + src_e + src_i + src_j) * div_0;
    dst_color += (src_a + src_b + src_g + src_f) * div_1;
    dst_color += (src_b + src_c + src_h + src_g) * div_1;
    dst_color += (src_f + src_g + src_l + src_k) * div_1;
    dst_color += (src_g + src_h + src_m + src_l) * div_1;

    dst_texture[dst_id] = float4(dst_color, 1.0f);
}

//
// Upsample pass.
//

ConstantBuffer<UpsampleBindings> g_upsample_bindings: register(b0);

FB_ATTRIBUTE(numthreads, DISPATCH_X, DISPATCH_Y, DISPATCH_Z)
void upsample_cs(fb::ComputeInput input) {
    const uint2 dst_id = input.dispatch_thread_id.xy;
    if (dst_id.x >= g_upsample_bindings.dst_texture_width
        || dst_id.y >= g_upsample_bindings.dst_texture_height) {
        return;
    }

    const uint src_dn_texture_id = g_upsample_bindings.src_dn_texture;
    const uint src_up_texture_id = g_upsample_bindings.src_up_texture;
    const uint src_texture_level = g_upsample_bindings.src_texture_level;
    const uint dst_texture_id = g_upsample_bindings.dst_texture;
    const uint dst_texture_level = g_upsample_bindings.dst_texture_level;
    const uint dst_texture_width = g_upsample_bindings.dst_texture_width;
    const uint dst_texture_height = g_upsample_bindings.dst_texture_height;
    const Texture2D<float4> src_dn_texture = ResourceDescriptorHeap[src_dn_texture_id];
    const Texture2D<float4> src_up_texture = ResourceDescriptorHeap[src_up_texture_id];
    const RWTexture2D<float4> dst_texture =
        ResourceDescriptorHeap[dst_texture_id + dst_texture_level];
    const SamplerState sampler = SamplerDescriptorHeap[(uint)GpuSampler::LinearClamp];

    const float2 dst_id_uv = 0.5f + float2(dst_id);
    const float2 dst_texel_size = 1.0f / float2(dst_texture_width, dst_texture_height);
    const float2 dst_uv = float2(dst_id_uv.x * dst_texel_size.x, dst_id_uv.y * dst_texel_size.y);

    float3 dst_dn_color = float3(0.0f, 0.0f, 0.0f);
    // clang-format off
    dst_dn_color += 1.0 * src_dn_texture.SampleLevel(sampler, dst_uv + dst_texel_size * float2(-1.0f, -1.0f), src_texture_level).rgb;
    dst_dn_color += 2.0 * src_dn_texture.SampleLevel(sampler, dst_uv + dst_texel_size * float2(-0.0f, -1.0f), src_texture_level).rgb;
    dst_dn_color += 1.0 * src_dn_texture.SampleLevel(sampler, dst_uv + dst_texel_size * float2(1.0f, -1.0f), src_texture_level).rgb;
    dst_dn_color += 2.0 * src_dn_texture.SampleLevel(sampler, dst_uv + dst_texel_size * float2(-1.0f, 0.0f), src_texture_level).rgb;
    dst_dn_color += 4.0 * src_dn_texture.SampleLevel(sampler, dst_uv + dst_texel_size * float2(0.0f, 0.0f), src_texture_level).rgb;
    dst_dn_color += 2.0 * src_dn_texture.SampleLevel(sampler, dst_uv + dst_texel_size * float2(1.0f, 0.0f), src_texture_level).rgb;
    dst_dn_color += 1.0 * src_dn_texture.SampleLevel(sampler, dst_uv + dst_texel_size * float2(-1.0f, 1.0f), src_texture_level).rgb;
    dst_dn_color += 2.0 * src_dn_texture.SampleLevel(sampler, dst_uv + dst_texel_size * float2(0.0f, 1.0f), src_texture_level).rgb;
    dst_dn_color += 1.0 * src_dn_texture.SampleLevel(sampler, dst_uv + dst_texel_size * float2(1.0f, 1.0f), src_texture_level).rgb;
    // clang-format on

    float3 dst_up_color = src_up_texture.SampleLevel(sampler, dst_uv, src_texture_level).rgb;
    float3 dst_color = 0.5f * (dst_dn_color + dst_up_color);

    dst_texture[dst_id] = float4(dst_color, 1.0f);
}

//
// Blit pass.
//

ConstantBuffer<BlitBindings> g_blit_bindings: register(b0);

struct BlitVertexOutput {
    float4 position: SV_Position;
    float2 texcoord: ATTRIBUTE0;
};

BlitVertexOutput blit_vs(fb::VertexInput input) {
    BlitVertexOutput output;
    fb::fullscreen_triangle(input.vertex_id, output.position, output.texcoord);
    return output;
}

fb::PixelOutput<1> blit_ps(BlitVertexOutput input) {
    const ConstantBuffer<Constants> constants = ResourceDescriptorHeap[g_blit_bindings.constants];
    const Texture2D scene_texture = ResourceDescriptorHeap[g_blit_bindings.scene_texture];
    const Texture2D bloom_texture = ResourceDescriptorHeap[g_blit_bindings.bloom_texture];
    const SamplerState sampler = SamplerDescriptorHeap[(uint)GpuSampler::LinearClamp];
    const bool tonemap = constants.tonemap != 0u;

    const float3 scene_color = scene_texture.Sample(sampler, input.texcoord).rgb;
    const float3 bloom_color = bloom_texture.Sample(sampler, input.texcoord).rgb;
    const float3 color = scene_color + bloom_color;
    const float3 tonemapped_color = tonemap ? fb::tonemap_aces(color) : color;

    fb::PixelOutput<1> output;
    output.color = float4(tonemapped_color, 1.0f);
    return output;
}
