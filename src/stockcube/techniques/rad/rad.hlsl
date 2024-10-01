#include <stockcube/techniques/rad/rad.hlsli>
#include <kitchen/gpu/samplers.hlsli>
#include <kitchen/kcn/core.hlsli>
#include <kitchen/kcn/brdf.hlsli>

const ConstantBuffer<AccBindings> g_acc_bindings: register(b0);
const ConstantBuffer<DivBindings> g_div_bindings: register(b0);

FB_ATTRIBUTE(numthreads, DISPATCH_X, DISPATCH_Y, DISPATCH_Z)
void acc_cs(fb::ComputeInput input) {
    // Global resources.
    const ConstantBuffer<Constants> constants = ResourceDescriptorHeap[g_acc_bindings.constants];
    const TextureCube<float3> cube_texture = ResourceDescriptorHeap[g_acc_bindings.cube_texture];
    const SamplerState cube_sampler = SamplerDescriptorHeap[g_acc_bindings.cube_sampler];
    const RWTexture2DArray<float4> rad_texture =
        ResourceDescriptorHeap[g_acc_bindings.rad_texture + g_acc_bindings.rad_texture_mip_id];
    const uint2 rad_texture_size = constants.rad_texture_size;
    const uint rad_texture_mip_count = constants.rad_texture_mip_count;
    const uint rad_sample_count = constants.rad_sample_count;
    const uint rad_sample_count_per_dispatch = constants.rad_sample_count_per_dispatch;

    // Indices.
    const uint dispatch_id = g_acc_bindings.dispatch_id;
    const uint2 src_id = input.dispatch_thread_id.xy;
    const uint face_id = input.dispatch_thread_id.z;
    const uint3 dst_id = input.dispatch_thread_id;
    const uint mip_id = g_acc_bindings.rad_texture_mip_id;

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
    const float3 dir = fb::cube_direction_from_dispatch_input(src_id, face_id, dst_texture_size);

    // Special: Clear if this is the first dispatch.
    if (dispatch_id == 0) {
        rad_texture[dst_id] = 0.0f.xxxx;
    }

    // Special: If this is the first level, just sample and return.
    if (mip_id == 0) {
        const float3 cube_sample = cube_texture.SampleLevel(cube_sampler, dir, 0);
        rad_texture[dst_id] = float4(cube_sample, 1.0f);
        return;
    }

    // Compute radiance.
    MaterialProperties material;
    material.base_color = 1.0f.xxx;
    material.metalness = 1.0f;
    material.roughness = roughness;
    const IndirectBrdfType brdf_type = IndirectBrdfType::Specular;
    float3 radiance = 0.0f.xxx;
    for (uint i = 0; i < rad_sample_count_per_dispatch; i++) {
        const uint global_id = i + dispatch_id * rad_sample_count_per_dispatch;
        const float2 u = fb::hammersley2d(global_id, rad_sample_count);
        const float3 n = dir;
        float3 ray_dir;
        float3 weight;
        if (evaluate_indirect_combined_brdf(u, n, n, n, material, brdf_type, ray_dir, weight)) {
            const float3 cube_sample = cube_texture.SampleLevel(cube_sampler, ray_dir, 0);
            radiance += weight * cube_sample;
        }
    }

    // Write.
    rad_texture[dst_id] += float4(radiance, 1.0f);
}

FB_ATTRIBUTE(numthreads, DISPATCH_X, DISPATCH_Y, DISPATCH_Z)
void div_cs(fb::ComputeInput input) {
    // Global resources.
    const ConstantBuffer<Constants> constants = ResourceDescriptorHeap[g_div_bindings.constants];
    const RWTexture2DArray<float4> acc_texture =
        ResourceDescriptorHeap[g_div_bindings.acc_texture + g_div_bindings.mip_id];
    const RWTexture2DArray<float4> div_texture =
        ResourceDescriptorHeap[g_div_bindings.div_texture + g_div_bindings.mip_id];
    const uint dispatch_id = g_div_bindings.dispatch_id;
    const uint rad_sample_count_per_dispatch = constants.rad_sample_count_per_dispatch;

    // Indices.
    const uint3 acc_id = input.dispatch_thread_id;
    const uint3 div_id = input.dispatch_thread_id;

    // Divide.
    float4 color;
    if (g_div_bindings.mip_id == 0) {
        color = acc_texture[acc_id];
    } else {
        const uint sample_count = (dispatch_id + 1) * rad_sample_count_per_dispatch;
        const float inv_sample_count = 1.0f / (float)sample_count;
        color = acc_texture[acc_id] * inv_sample_count;
    }

    // Write.
    div_texture[div_id] = color;
}
