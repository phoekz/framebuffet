#include <stockcube/techniques/cfr/cfr.hlsli>
#include <kitchen/gpu/samplers.hlsli>
#include <kitchen/graphics/core.hlsli>

ConstantBuffer<Bindings> g_bindings: register(b0);

FB_ATTRIBUTE(numthreads, DISPATCH_X, DISPATCH_Y, DISPATCH_Z)
void cs(fb::ComputeInput input) {
    // Global resources.
    ConstantBuffer<Constants> constants = ResourceDescriptorHeap[g_bindings.constants];
    const uint2 cube_texture_size = constants.cube_texture_size;
    Texture2D<float4> rect_texture = ResourceDescriptorHeap[g_bindings.rect_texture];
    SamplerState rect_sampler = SamplerDescriptorHeap[g_bindings.rect_sampler];
    RWTexture2DArray<float4> cube_texture = ResourceDescriptorHeap[g_bindings.cube_texture];

    // Indices.
    const uint2 src_id = input.dispatch_thread_id.xy;
    const uint face_id = input.dispatch_thread_id.z;
    const uint3 dst_id = input.dispatch_thread_id;

    // Direction.
    const float3 dir = fb::cube_direction_from_dispatch_input(src_id, face_id, cube_texture_size);

    // Normalized latitude/longitude from direction.
    const float lon = 0.5f + atan2(dir.z, dir.x) / (2.0f * FB_PI);
    const float lat = acos(dir.y) / FB_PI;

    // Sample.
    float3 color = rect_texture.Sample(rect_sampler, float2(lon, lat)).rgb;

    // Write.
    cube_texture[dst_id] = float4(color, 1.0f);
}
