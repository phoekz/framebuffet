#include <stockcube/techniques/irr/irr.hlsli>
#include <kitchen/gpu/samplers.hlsli>
#include <kitchen/graphics/core.hlsli>
#include <kitchen/graphics/brdf.hlsli>

ConstantBuffer<AccBindings> g_acc_bindings: register(b0);
ConstantBuffer<DivBindings> g_div_bindings: register(b0);

FB_ATTRIBUTE(numthreads, DISPATCH_X, DISPATCH_Y, DISPATCH_Z)
void acc_cs(FbComputeInput input) {
    // Global resources.
    ConstantBuffer<Constants> constants = ResourceDescriptorHeap[g_acc_bindings.constants];
    TextureCube<float3> cube_texture = ResourceDescriptorHeap[g_acc_bindings.cube_texture];
    SamplerState cube_sampler = SamplerDescriptorHeap[g_acc_bindings.cube_sampler];
    RWTexture2DArray<float4> irr_texture = ResourceDescriptorHeap[g_acc_bindings.irr_texture];
    const uint2 irr_texture_size = constants.irr_texture_size;
    const uint irr_sample_count = constants.irr_sample_count;
    const uint irr_sample_count_per_dispatch = constants.irr_sample_count_per_dispatch;

    // Indices.
    const uint dispatch_id = g_acc_bindings.dispatch_id;
    const uint2 src_id = input.dispatch_thread_id.xy;
    const uint face_id = input.dispatch_thread_id.z;
    const uint3 dst_id = input.dispatch_thread_id;

    // Direction.
    const float3 dir = fb_cube_direction_from_dispatch_input(src_id, face_id, irr_texture_size);

    // Special: Clear if this is the first dispatch.
    if (dispatch_id == 0) {
        irr_texture[dst_id] = 0.0f.xxxx;
    }

    // Accumulate irradiance.
    MaterialProperties material;
    material.base_color = 1.0f.xxx;
    material.metalness = 0.0f;
    material.roughness = 1.0f;
    float3 irradiance = 0.0f.xxx;
    for (uint i = 0; i < irr_sample_count_per_dispatch; i++) {
        const uint global_i = i + dispatch_id * irr_sample_count_per_dispatch;
        const float2 u = fb_hammersley2d(global_i, irr_sample_count);
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
        const float3 cube_sample = cube_texture.SampleLevel(cube_sampler, ray_dir, 0);
        irradiance += weight * cube_sample;
    }

    // Write.
    irr_texture[dst_id] += float4(irradiance, 1.0f);
}

FB_ATTRIBUTE(numthreads, DISPATCH_X, DISPATCH_Y, DISPATCH_Z)
void div_cs(FbComputeInput input) {
    // Global resources.
    ConstantBuffer<Constants> constants = ResourceDescriptorHeap[g_div_bindings.constants];
    RWTexture2DArray<float4> acc_texture = ResourceDescriptorHeap[g_div_bindings.acc_texture];
    RWTexture2DArray<float4> div_texture = ResourceDescriptorHeap[g_div_bindings.div_texture];
    const uint dispatch_id = g_div_bindings.dispatch_id;
    const uint irr_sample_count_per_dispatch = constants.irr_sample_count_per_dispatch;

    // Indices.
    const uint3 acc_id = input.dispatch_thread_id;
    const uint3 div_id = input.dispatch_thread_id;

    // Divide.
    const uint sample_count = (dispatch_id + 1) * irr_sample_count_per_dispatch;
    const float inv_sample_count = 1.0f / (float)sample_count;
    const float4 color = acc_texture[acc_id] * inv_sample_count;

    // Write.
    div_texture[div_id] = color;
}
