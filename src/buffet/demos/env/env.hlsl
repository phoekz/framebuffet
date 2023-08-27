#include <gpu/samplers.hlsli>
#include <demos/core.hlsli>
#include <demos/brdf.hlsli>
#include <demos/env/env.hlsli>

// Source: https://knarkowicz.wordpress.com/2016/01/06/aces-filmic-tone-mapping-curve/
float3 tonemap_aces(float3 x) {
    const float a = 2.51f;
    const float b = 0.03f;
    const float c = 2.43f;
    const float d = 0.59f;
    const float e = 0.14f;
    return saturate((x * (a * x + b)) / (x * (c * x + d) + e));
}

//
// Compute.
//

ConstantBuffer<ComputeBindings> g_compute_bindings: register(b0);

FB_ATTRIBUTE(numthreads, CFR_DISPATCH_X, CFR_DISPATCH_Y, CFR_DISPATCH_Z)
void cfr_cs(FbComputeInput input) {
    // Global resources.
    ConstantBuffer<ComputeConstants> constants =
        ResourceDescriptorHeap[g_compute_bindings.constants];
    Texture2D<float4> rect_texture = ResourceDescriptorHeap[g_compute_bindings.rect_texture];
    SamplerState rect_sampler = SamplerDescriptorHeap[(uint)GpuSampler::LinearClamp];
    RWTexture2DArray<float4> cube_texture = ResourceDescriptorHeap[g_compute_bindings.cube_texture];

    // Indices.
    const uint2 src_id = input.dispatch_thread_id.xy;
    const uint face_id = input.dispatch_thread_id.z;
    const uint3 dst_id = input.dispatch_thread_id;

    // Direction from dispatch indices.
    const float2 size = (float2)constants.cube_texture_size;
    float2 uv = (float2(src_id) + 0.5f) / size;
    uv = 2.0f * uv - 1.0f;
    uv.y *= -1.0f;
    float3 dir;
    switch (face_id) {
        case 0: dir = float3(1.0f, uv.y, -uv.x); break;
        case 1: dir = float3(-1.0f, uv.y, uv.x); break;
        case 2: dir = float3(uv.x, 1.0f, -uv.y); break;
        case 3: dir = float3(uv.x, -1.0f, uv.y); break;
        case 4: dir = float3(uv.x, uv.y, 1.0); break;
        case 5: dir = float3(-uv.x, uv.y, -1.0); break;
    }
    dir = normalize(dir);

    // Latitude/longitude from direction.
    const float lon = 0.5f + atan2(dir.z, dir.x) / (2.0f * FB_PI);
    const float lat = acos(dir.y) / FB_PI;

    // Sample rect texture.
    const float3 color = rect_texture.Sample(rect_sampler, float2(lon, lat)).rgb;

    // Write to cube texture.
    cube_texture[dst_id] = float4(color, 1.0f);
}

float2 hammersley2d(uint i, uint n) {
    // From: http://holger.dammertz.org/stuff/notes_HammersleyOnHemisphere.html
    uint bits = (i << 16u) | (i >> 16u);
    bits = ((bits & 0x55555555u) << 1u) | ((bits & 0xaaaaaaaau) >> 1u);
    bits = ((bits & 0x33333333u) << 2u) | ((bits & 0xccccccccu) >> 2u);
    bits = ((bits & 0x0f0f0f0fu) << 4u) | ((bits & 0xf0f0f0f0u) >> 4u);
    bits = ((bits & 0x00ff00ffu) << 8u) | ((bits & 0xff00ff00u) >> 8u);
    return float2(float(i) / float(n), float(bits) * 2.3283064365386963e-10f);
}

FB_ATTRIBUTE(numthreads, LUT_DISPATCH_X, LUT_DISPATCH_Y, LUT_DISPATCH_Z)
void lut_cs(FbComputeInput input) {
    // Global resources.
    ConstantBuffer<ComputeConstants> constants =
        ResourceDescriptorHeap[g_compute_bindings.constants];
    RWTexture2D<float2> lut_texture = ResourceDescriptorHeap[g_compute_bindings.lut_texture];

    // Indices.
    const uint2 dst_id = input.dispatch_thread_id.xy;

    // Texcoords.
    const float2 size = (float2)constants.lut_texture_size;
    const float2 texcoord = (float2(dst_id) + 0.5f) / size;
    const float ndotv = texcoord.x;
    const float roughness = 1.0f - texcoord.y;

    // Compute LUT: scale and bias for F0.
    const uint sample_count = 1024;
    const float3 specular_f0 = 0.04f.xxx;
    const float specular_f90 = shadowed_f90(specular_f0);
    const float3 v = float3(sqrt(1.0f - ndotv * ndotv), 0.0f, ndotv);
    const float3 n = float3(0.0f, 0.0f, 1.0f);
    const float alpha = roughness * roughness;
    const float alpha_squared = alpha * alpha;
    float sum_scale = 0.0f;
    float sum_bias = 0.0f;
    for (uint i = 0; i < sample_count; i++) {
        const float2 u = hammersley2d(i, sample_count);
        const float3 h = sample_ggx_vndf(v, alpha.xx, u);
        const float3 l = reflect(-v, h);
        const float hdotl = clamp(dot(h, l), 0.00001f, 1.0f);
        const float ndotl = clamp(dot(n, l), 0.00001f, 1.0f);
        const float ndoth = clamp(dot(n, h), 0.00001f, 1.0f);
        const float vdoth = clamp(dot(v, h), 0.00001f, 1.0f);
        if (ndotl > 0.0f) {
            const float fresnel = evaluate_fresnel(specular_f0, specular_f90, vdoth).x;
            const float weight =
                specular_sample_weight_ggx_vndf(alpha, alpha_squared, ndotl, ndotv, hdotl, ndoth);
            sum_scale += (1.0 - fresnel) * weight;
            sum_bias += fresnel * weight;
        }
    }
    sum_scale /= float(sample_count);
    sum_bias /= float(sample_count);

    lut_texture[dst_id] = float2(sum_scale, sum_bias);
}

//
// Background.
//

struct BackgroundVertexOutput {
    float4 position: SV_Position;
    float3 normal: ATTRIBUTE0;
    float2 texcoord: ATTRIBUTE1;
    float3 direction: ATTRIBUTE2;
};

ConstantBuffer<BackgroundBindings> g_background_bindings: register(b0);

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
    output.direction.z *= -1.0f; // Handedness flip.
    return output;
}

FbPixelOutput1 background_ps(BackgroundVertexOutput input) {
    ConstantBuffer<BackgroundConstants> constants =
        ResourceDescriptorHeap[g_background_bindings.constants];
    TextureCube<float4> texture = ResourceDescriptorHeap[g_background_bindings.texture];
    SamplerState samp = SamplerDescriptorHeap[(uint)GpuSampler::LinearWrap];
    float3 color = texture.Sample(samp, input.direction).rgb;

    if (constants.tonemap) {
        color = tonemap_aces(color);
    }

    FbPixelOutput1 output;
    output.color = float4(color, 1.0f);
    return output;
}

//
// Screen.
//

struct ScreenVertexOutput {
    float4 position: SV_Position;
    float2 texcoord: ATTRIBUTE0;
};

ConstantBuffer<ScreenBindings> g_screen_bindings: register(b0);

ScreenVertexOutput screen_vs(FbVertexInput input) {
    ConstantBuffer<ScreenConstants> constants = ResourceDescriptorHeap[g_screen_bindings.constants];
    StructuredBuffer<ScreenVertex> vertices = ResourceDescriptorHeap[g_screen_bindings.vertices];
    ScreenVertex vertex = vertices[input.vertex_id];

    float2 offset = g_screen_bindings.screen_offset;
    float2 scale = g_screen_bindings.screen_scale;
    float2 position = (scale * vertex.position) + offset;

    ScreenVertexOutput output;
    output.position = mul(constants.transform, float4(position, 0.0f, 1.0f));
    output.texcoord = vertex.texcoord;
    return output;
}

FbPixelOutput1 screen_ps(ScreenVertexOutput input) {
    ConstantBuffer<ScreenConstants> constants = ResourceDescriptorHeap[g_screen_bindings.constants];
    Texture2DArray<float4> texture = ResourceDescriptorHeap[g_screen_bindings.texture];
    SamplerState samp = SamplerDescriptorHeap[(uint)GpuSampler::LinearClamp];
    uint texture_slice = g_screen_bindings.texture_slice;
    float3 color = texture.SampleLevel(samp, float3(input.texcoord, texture_slice), 0).rgb;

    if (constants.tonemap) {
        color = tonemap_aces(color);
    }

    FbPixelOutput1 output;
    output.color = float4(color, 1.0f);
    return output;
}
