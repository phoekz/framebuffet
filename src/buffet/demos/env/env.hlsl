#include <kitchen/gpu/samplers.hlsli>
#include <kitchen/graphics/core.hlsli>
#include <buffet/demos/brdf.hlsli>
#include <buffet/demos/env/env.hlsli>

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

float3 direction_from_dispatch_input(uint2 src_id, uint face_id, uint2 face_size) {
    float2 p = (float2(src_id) + 0.5f) / face_size;
    p = 2.0f * p - 1.0f;
    p.y *= -1.0f;
    float3 dir;
    switch (face_id) {
        case 0: dir = float3(1.0f, p.y, -p.x); break;
        case 1: dir = float3(-1.0f, p.y, p.x); break;
        case 2: dir = float3(p.x, 1.0f, -p.y); break;
        case 3: dir = float3(p.x, -1.0f, p.y); break;
        case 4: dir = float3(p.x, p.y, 1.0); break;
        case 5: dir = float3(-p.x, p.y, -1.0); break;
    }
    dir = normalize(dir);
    return dir;
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

ConstantBuffer<ComputeBindings> g_compute_bindings: register(b0);

FB_ATTRIBUTE(numthreads, CFR_DISPATCH_X, CFR_DISPATCH_Y, CFR_DISPATCH_Z)
void cfr_cs(FbComputeInput input) {
    // Global resources.
    ConstantBuffer<ComputeConstants> constants =
        ResourceDescriptorHeap[g_compute_bindings.constants];
    const uint2 cube_texture_size = constants.cube_texture_size;
    Texture2D<float4> rect_texture = ResourceDescriptorHeap[g_compute_bindings.rect_texture];
    SamplerState rect_sampler = SamplerDescriptorHeap[(uint)GpuSampler::LinearClamp];
    RWTexture2DArray<float4> cube_texture = ResourceDescriptorHeap[g_compute_bindings.cube_texture];

    // Indices.
    const uint2 src_id = input.dispatch_thread_id.xy;
    const uint face_id = input.dispatch_thread_id.z;
    const uint3 dst_id = input.dispatch_thread_id;

    // Direction.
    const float3 dir = direction_from_dispatch_input(src_id, face_id, cube_texture_size);

    // Latitude/longitude from direction.
    const float lon = 0.5f + atan2(dir.x, dir.z) / (2.0f * FB_PI);
    const float lat = acos(dir.y) / FB_PI;

    // Sample rect texture.
    float3 color = rect_texture.Sample(rect_sampler, float2(lon, lat)).rgb;

    // Write to cube texture.
    cube_texture[dst_id] = float4(color, 1.0f);
}

FB_ATTRIBUTE(numthreads, LUT_DISPATCH_X, LUT_DISPATCH_Y, LUT_DISPATCH_Z)
void lut_cs(FbComputeInput input) {
    // Global resources.
    ConstantBuffer<ComputeConstants> constants =
        ResourceDescriptorHeap[g_compute_bindings.constants];
    const uint2 lut_texture_size = constants.lut_texture_size;
    RWTexture2D<float2> lut_texture = ResourceDescriptorHeap[g_compute_bindings.lut_texture];

    // Indices.
    const uint2 dst_id = input.dispatch_thread_id.xy;

    // Texcoords.
    const float2 texture_size = (float2)lut_texture_size;
    const float2 texcoord = (float2(dst_id) + 0.5f) / texture_size;
    const float ndotv = texcoord.x;
    const float roughness = 1.0f - texcoord.y;

    // Compute LUT: scale and bias for F0.
    const uint sample_count = constants.lut_sample_count;
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

FB_ATTRIBUTE(numthreads, IRR_DISPATCH_X, IRR_DISPATCH_Y, IRR_DISPATCH_Z)
void irr_cs(FbComputeInput input) {
    // Global resources.
    ConstantBuffer<ComputeConstants> constants =
        ResourceDescriptorHeap[g_compute_bindings.constants];
    const uint2 irr_texture_size = constants.irr_texture_size;
    TextureCube<float4> cube_texture = ResourceDescriptorHeap[g_compute_bindings.cube_texture];
    SamplerState cube_sampler = SamplerDescriptorHeap[(uint)GpuSampler::LinearClamp];
    RWTexture2DArray<float4> irr_texture = ResourceDescriptorHeap[g_compute_bindings.irr_texture];

    // Indices.
    const uint2 src_id = input.dispatch_thread_id.xy;
    const uint face_id = input.dispatch_thread_id.z;
    const uint3 dst_id = input.dispatch_thread_id;

    // Direction.
    const float3 dir = direction_from_dispatch_input(src_id, face_id, irr_texture_size);

    // Compute irradiance.
    MaterialProperties material;
    material.base_color = 1.0f.xxx;
    material.metalness = 0.0f;
    material.roughness = 1.0f;
    const uint sample_count = constants.irr_sample_count;
    const float inv_sample_count = 1.0f / float(sample_count);
    const uint dispatch_sample_count = constants.irr_dispatch_sample_count;
    float3 irradiance = 0.0f.xxx;
    for (uint i = 0; i < dispatch_sample_count; i++) {
        const float2 u = hammersley2d(
            dispatch_sample_count * g_compute_bindings.irr_dispatch_index + i,
            sample_count
        );
        const float3 n = dir;
        const float4 q_rotation_to_z = get_rotation_to_z_axis(n);
        const float4 q_rotation_from_z = invert_rotation(q_rotation_to_z);
        const float3 local_v = rotate_point(q_rotation_to_z, n);
        const float3 local_n = float3(0.0f, 0.0f, 1.0f);
        const float3 local_ray_dir = sample_hemisphere(u);
        const BrdfData data = prepare_brdf_data(local_n, local_ray_dir, local_v, material);
        const float3 f = evaluate_fresnel(data.specular_f0, shadowed_f90(data.specular_f0), 1.0f);
        const float3 weight = (1.0f - f) * disney_diffuse(data);
        const float3 ray_dir = normalize(rotate_point(q_rotation_from_z, local_ray_dir));
        irradiance +=
            inv_sample_count * weight * cube_texture.SampleLevel(cube_sampler, ray_dir, 0).rgb;
    }

    // Write to irradiance texture.
    irr_texture[dst_id] += float4(irradiance, 1.0f);
}

FB_ATTRIBUTE(numthreads, RAD_DISPATCH_X, RAD_DISPATCH_Y, RAD_DISPATCH_Z)
void rad_cs(FbComputeInput input) {
    // Global resources.
    ConstantBuffer<ComputeConstants> constants =
        ResourceDescriptorHeap[g_compute_bindings.constants];
    const uint2 rad_texture_size = constants.rad_texture_size;
    const uint rad_texture_mip_count = constants.rad_texture_mip_count;
    TextureCube<float4> cube_texture = ResourceDescriptorHeap[g_compute_bindings.cube_texture];
    SamplerState cube_sampler = SamplerDescriptorHeap[(uint)GpuSampler::LinearClamp];
    RWTexture2DArray<float4> rad_texture =
        ResourceDescriptorHeap[g_compute_bindings.rad_texture + g_compute_bindings.rad_mip_id];

    // Indices.
    const uint2 src_id = input.dispatch_thread_id.xy;
    const uint face_id = input.dispatch_thread_id.z;
    const uint3 dst_id = input.dispatch_thread_id;
    const uint mip_id = g_compute_bindings.rad_mip_id;

    // Bounds check.
    if (dst_id.x >= rad_texture_size.x || dst_id.y >= rad_texture_size.y) {
        return;
    }

    // Texture size.
    const uint2 dst_texture_size =
        uint2(max(1u, rad_texture_size.x >> mip_id), max(1u, rad_texture_size.y >> mip_id));

    // Roughness.
    const float roughness = float(mip_id) / float(rad_texture_mip_count - 1);

    // Direction.
    const float3 dir = direction_from_dispatch_input(src_id, face_id, dst_texture_size);

    // Compute radiance.
    MaterialProperties material;
    material.base_color = 1.0f.xxx;
    material.metalness = 1.0f;
    material.roughness = roughness;
    const IndirectBrdfType brdf_type = IndirectBrdfType::Specular;
    const uint sample_count = constants.rad_sample_count;
    const float inv_sample_count = 1.0f / float(sample_count);
    const uint dispatch_sample_count = constants.rad_dispatch_sample_count;
    float3 radiance = 0.0f.xxx;
    for (uint i = 0; i < dispatch_sample_count; i++) {
        const float2 u = hammersley2d(
            dispatch_sample_count * g_compute_bindings.rad_dispatch_index + i,
            sample_count
        );
        const float3 n = dir;
        float3 ray_dir;
        float3 weight;
        if (evaluate_indirect_combined_brdf(u, n, n, n, material, brdf_type, ray_dir, weight)) {
            radiance +=
                inv_sample_count * weight * cube_texture.SampleLevel(cube_sampler, ray_dir, 0).rgb;
        }
    }

    // Write to radiance texture.
    rad_texture[dst_id] += float4(radiance, 1.0f);
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

    const float roughness = constants.roughness;
    const uint mip_count = constants.mip_count;
    const float mip_level = roughness * float(mip_count - 1);
    float3 color = texture.SampleLevel(samp, input.direction, mip_level).rgb;

    if (constants.tonemap) {
        color = tonemap_aces(color);
    }

    color *= constants.exposure;

    FbPixelOutput1 output;
    output.color = float4(color, 1.0f);
    return output;
}

//
// Model.
//

struct ModelVertexOutput {
    float4 position: SV_Position;
    float3 normal: ATTRIBUTE0;
    float3 world_position: ATTRIBUTE1;
};

ConstantBuffer<ModelBindings> g_model_bindings: register(b0);

ModelVertexOutput model_vs(FbVertexInput input) {
    ConstantBuffer<ModelConstants> constants = ResourceDescriptorHeap[g_model_bindings.constants];
    StructuredBuffer<FbVertex> vertices = ResourceDescriptorHeap[g_model_bindings.vertices];
    FbVertex vertex = vertices[input.vertex_id];

    ModelVertexOutput output;
    output.position = mul(constants.transform, float4(vertex.position, 1.0f));
    output.normal = vertex.normal;
    output.world_position = vertex.position;
    return output;
}

FbPixelOutput1 model_ps(ModelVertexOutput input) {
    ConstantBuffer<ModelConstants> constants = ResourceDescriptorHeap[g_model_bindings.constants];
    Texture2D<float2> lut_texture = ResourceDescriptorHeap[g_model_bindings.lut_texture];
    TextureCube<float3> irr_texture = ResourceDescriptorHeap[g_model_bindings.irr_texture];
    TextureCube<float3> rad_texture = ResourceDescriptorHeap[g_model_bindings.rad_texture];
    SamplerState samp = SamplerDescriptorHeap[(uint)GpuSampler::LinearClamp];

    const float3 n = normalize(input.normal);
    const float3 v = normalize(constants.camera_position - input.world_position);
    const float3 r = reflect(-v, n);
    const float ndotv = saturate(dot(n, v));
    const float roughness = constants.roughness;
    const float mip_count = constants.mip_count;
    const float radiance_lod = roughness * float(mip_count - 1);

    const float2 f0_scale_bias = lut_texture.Sample(samp, float2(ndotv, roughness));
    const float f0_scale = f0_scale_bias.x;
    const float f0_bias = f0_scale_bias.y;
    const float3 irradiance = irr_texture.SampleLevel(samp, r, 0);
    const float3 radiance = rad_texture.SampleLevel(samp, r, radiance_lod);

    const float3 base_color = float3(1.0f, 1.0f, 1.0f);
    const float3 specular_f0 = lerp(0.04f.xxx, base_color, constants.metallic);

    const float3 diffuse = irradiance * base_color;
    const float3 specular = radiance * (specular_f0 * f0_scale + f0_bias);
    float3 color = diffuse + specular;

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
    SamplerState samp = SamplerDescriptorHeap[(uint)GpuSampler::PointClamp];
    const uint texture_face_id = g_screen_bindings.texture_face_id;
    const uint texture_mip_id = g_screen_bindings.texture_mip_id;
    float3 color =
        texture.SampleLevel(samp, float3(input.texcoord, texture_face_id), texture_mip_id).rgb;

    if (constants.tonemap) {
        color = tonemap_aces(color);
    }

    color *= constants.exposure;

    FbPixelOutput1 output;
    output.color = float4(color, 1.0f);
    return output;
}
