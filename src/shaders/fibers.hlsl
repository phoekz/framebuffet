#include "shaders/core.hlsli"
#include "shaders/samplers.hlsli"
#include "shaders/fibers.hlsli"

struct Constants {
    float4x4 clip_from_world;
    float4x4 view_from_clip;
    float4x4 view_from_world;
    float2 window_size;
    float delta_time;
    float light_speed;
    float light_range;
    float light_intensity;
    float debug_opacity;
};

struct Light {
    float3 position;
    float3 color;
    float speed_variation;
};

struct Vertex {
    float3 position;
    float3 normal;
    float2 texcoord;
};

ConstantBuffer<Bindings> g_bindings: register(b0);

FB_ATTRIBUTE(numthreads, SIM_DISPATCH_SIZE, 1, 1)
void sim_cs(FbComputeInput input) {
    ConstantBuffer<Constants> constants = ResourceDescriptorHeap[g_bindings.constants];
    RWStructuredBuffer<Light> lights = ResourceDescriptorHeap[g_bindings.lights];

    uint id = input.dispatch_thread_id.x;
    Light light = lights[id];
    float3 velocity = float3(1.0f, 0.0f, 0.0f);
    velocity *= light.speed_variation;
    velocity *= constants.light_speed;
    velocity *= constants.delta_time;
    light.position += velocity;
    if (light.position.x > PLANE_RADIUS_X) {
        light.position.x = -PLANE_RADIUS_X;
    }
    lights[id] = light;
}

FB_ATTRIBUTE(numthreads, 1, 1, 1)
void reset_cs(FbComputeInput input) {
    RWStructuredBuffer<uint> light_indices_count =
        ResourceDescriptorHeap[g_bindings.light_indices_count];
    light_indices_count[0] = 0;
}

groupshared float4 gs_frustum_planes[4];
groupshared uint gs_light_indices[LIGHT_COUNT];
groupshared uint gs_light_count;
groupshared uint gs_light_offset;

float3 view_point_from_clip(float4x4 view_from_clip, float clip_x, float clip_y) {
    const float4 clip = float4(clip_x, clip_y, -1.0f, 1.0f);
    const float4 view = mul(view_from_clip, clip);
    return view.xyz / view.w;
}

FB_ATTRIBUTE(numthreads, CULL_DISPATCH_SIZE, CULL_DISPATCH_SIZE, 1)
void cull_cs(FbComputeInput input) {
    ConstantBuffer<Constants> constants = ResourceDescriptorHeap[g_bindings.constants];
    StructuredBuffer<Light> lights = ResourceDescriptorHeap[g_bindings.lights];
    RWTexture2D<uint> light_counts_texture =
        ResourceDescriptorHeap[g_bindings.light_counts_texture];
    RWTexture2D<uint> light_offsets_texture =
        ResourceDescriptorHeap[g_bindings.light_offsets_texture];
    RWStructuredBuffer<uint> light_indices = ResourceDescriptorHeap[g_bindings.light_indices];
    RWStructuredBuffer<uint> light_indices_count =
        ResourceDescriptorHeap[g_bindings.light_indices_count];

    // Compute frustum planes.
    if (input.group_index == 0) {
        const uint screen_tile_size = CULL_DISPATCH_SIZE;
        const float screen_x0 = (float)(input.group_id.x * screen_tile_size);
        const float screen_y0 = (float)(input.group_id.y * screen_tile_size);
        const float screen_x1 = (float)(screen_x0 + screen_tile_size);
        const float screen_y1 = (float)(screen_y0 + screen_tile_size);

        const float2 window_size = constants.window_size;
        const float clip_x0 = (2.0f * screen_x0 / window_size.x - 1.0f);
        const float clip_y0 = -(2.0f * screen_y0 / window_size.y - 1.0f);
        const float clip_x1 = (2.0f * screen_x1 / window_size.x - 1.0f);
        const float clip_y1 = -(2.0f * screen_y1 / window_size.y - 1.0f);

        const float4x4 view_from_clip = constants.view_from_clip;
        const float3 view_point_0 = view_point_from_clip(view_from_clip, clip_x0, clip_y0);
        const float3 view_point_1 = view_point_from_clip(view_from_clip, clip_x1, clip_y0);
        const float3 view_point_2 = view_point_from_clip(view_from_clip, clip_x0, clip_y1);
        const float3 view_point_3 = view_point_from_clip(view_from_clip, clip_x1, clip_y1);

        const float3 origin = float3(0.0f, 0.0f, 0.0f);
        gs_frustum_planes[0] = fb_plane_from_points(origin, view_point_2, view_point_0);
        gs_frustum_planes[1] = fb_plane_from_points(origin, view_point_1, view_point_3);
        gs_frustum_planes[2] = fb_plane_from_points(origin, view_point_0, view_point_1);
        gs_frustum_planes[3] = fb_plane_from_points(origin, view_point_3, view_point_2);

        gs_light_count = 0;
        gs_light_offset = 0;
    }
    GroupMemoryBarrierWithGroupSync();

    // Todo: calculate min/max depth for each tile with atomic min/max.

    // Cull lights.
    uint hit_mask = 0;
    if (input.group_index < LIGHT_COUNT) {
        const Light light = lights[input.group_index];
        const float light_radius = constants.light_range;
        const float4x4 view_from_world = constants.view_from_world;
        for (uint i = 0; i < 4; ++i) {
            const float4 plane = gs_frustum_planes[i];
            const float3 light_position = mul(view_from_world, float4(light.position, 1.0f)).xyz;
            const bool hit = fb_sphere_inside_plane(plane, light_position, light_radius);
            hit_mask |= (uint)hit << i;
        }
        if (hit_mask == 0) {
            uint dst = 0;
            InterlockedAdd(gs_light_count, 1, dst);
            if (dst < LIGHT_CAPACITY_PER_TILE) {
                gs_light_indices[dst] = input.group_index;
            }
        }
    }
    GroupMemoryBarrierWithGroupSync();

    // Write.
    uint light_offset = 0;
    if (input.group_index == 0) {
        if (gs_light_count != 0) {
            InterlockedAdd(light_indices_count[0], gs_light_count, light_offset);
        }
        light_counts_texture[input.group_id.xy] = gs_light_count;
        light_offsets_texture[input.group_id.xy] = light_offset;
        gs_light_offset = light_offset;
    }
    GroupMemoryBarrierWithGroupSync();
    light_offset = gs_light_offset;
    for (uint i = input.group_index; i < gs_light_count; i += 64) {
        light_indices[light_offset + i] = gs_light_indices[i];
    }
}

struct LightVertexOutput {
    float4 position: SV_Position;
    float3 normal: ATTRIBUTE0;
    float2 texcoord: ATTRIBUTE1;
    float3 color: ATTRIBUTE2;
};

LightVertexOutput light_vs(FbVertexInput input) {
    ConstantBuffer<Constants> constants = ResourceDescriptorHeap[g_bindings.constants];
    StructuredBuffer<Light> lights = ResourceDescriptorHeap[g_bindings.lights];
    StructuredBuffer<Vertex> vertices = ResourceDescriptorHeap[g_bindings.vertices];

    Light light = lights[input.instance_id];
    Vertex vertex = vertices[input.vertex_id];

    float3 position = vertex.position * constants.light_range + light.position;

    LightVertexOutput output;
    output.position = mul(constants.clip_from_world, float4(position, 1.0f));
    output.normal = vertex.normal;
    output.texcoord = vertex.texcoord;
    output.color = light.color;
    return output;
}

FbPixelOutput1 light_ps(LightVertexOutput input) {
    FbPixelOutput1 output;
    output.color = float4(1.0f, 1.0f, 1.0f, 1.0f);
    return output;
}

struct PlaneVertexOutput {
    float4 position: SV_Position;
    float3 normal: ATTRIBUTE0;
    float2 texcoord: ATTRIBUTE1;
    float3 world_position: ATTRIBUTE2;
    float4 ndc_position: ATTRIBUTE3;
};

PlaneVertexOutput plane_vs(FbVertexInput input) {
    ConstantBuffer<Constants> constants = ResourceDescriptorHeap[g_bindings.constants];
    StructuredBuffer<Vertex> vertices = ResourceDescriptorHeap[g_bindings.vertices];

    Vertex vertex = vertices[input.vertex_id];

    PlaneVertexOutput output;
    output.position = mul(constants.clip_from_world, float4(vertex.position, 1.0f));
    output.normal = vertex.normal;
    output.texcoord = vertex.texcoord;
    output.world_position = vertex.position;
    output.ndc_position = output.position;
    return output;
}

FbPixelOutput1 plane_ps(PlaneVertexOutput input) {
    ConstantBuffer<Constants> constants = ResourceDescriptorHeap[g_bindings.constants];
    StructuredBuffer<Light> lights = ResourceDescriptorHeap[g_bindings.lights];
    Texture2D<uint> light_counts_texture = ResourceDescriptorHeap[g_bindings.light_counts_texture];
    Texture2D<uint> light_offsets_texture =
        ResourceDescriptorHeap[g_bindings.light_offsets_texture];
    StructuredBuffer<uint> light_indices = ResourceDescriptorHeap[g_bindings.light_indices];

    float3 color = float3(0.02f, 0.02f, 0.02f);

    const float2 window_size = constants.window_size;
    const float2 tile_size = (float2)CULL_TILE_SIZE;
    const float2 screen_coord = fb_screen_coord_from_ndc(input.ndc_position);
    const float2 tile_pixel = screen_coord * window_size / tile_size;
    const uint2 tile_index = floor(tile_pixel);
    const uint light_count = light_counts_texture[tile_index];
    const uint light_offset = light_offsets_texture[tile_index];
    for (uint i = 0; i < light_count; i++) {
        const uint light_index = light_indices[light_offset + i];
        const Light light = lights[light_index];

        float3 light_from_surface = light.position - input.world_position;
        float distance = max(0.001f, length(light_from_surface));
        float attenuation =
            saturate(1.0 - fb_pow4(distance / constants.light_range)) / fb_pow2(distance);
        float3 light_color = attenuation * light.color;
        color += constants.light_intensity * light_color;
    }

    FbPixelOutput1 output;
    output.color = float4(color, 1.0f);
    return output;
}

struct DebugVertexOutput {
    float4 position: SV_Position;
    float2 texcoord: ATTRIBUTE0;
};

DebugVertexOutput debug_vs(FbVertexInput input) {
    DebugVertexOutput output;
    fb_fullscreen_triangle(input.vertex_id, output.position, output.texcoord);
    return output;
}

FbPixelOutput1 debug_ps(DebugVertexOutput input) {
    ConstantBuffer<Constants> constants = ResourceDescriptorHeap[g_bindings.constants];
    RWTexture2D<uint> light_counts_texture =
        ResourceDescriptorHeap[g_bindings.light_counts_texture];
    Texture2D<float3> heatmap_texture = ResourceDescriptorHeap[g_bindings.heatmap_texture];
    SamplerState heatmap_sampler = SamplerDescriptorHeap[(uint)GpuSamplerType::LinearClamp];

    const float2 window_size = constants.window_size;
    const float2 tile_size = (float2)CULL_TILE_SIZE;
    const float2 tile_pixel = input.texcoord * window_size / tile_size;
    const uint2 tile_index = floor(tile_pixel);
    uint light_count = light_counts_texture[tile_index];
    float shade = saturate((float)light_count / 5.0f);
    float3 color = heatmap_texture.Sample(heatmap_sampler, float2(shade, 0.5f));

    FbPixelOutput1 output;
    output.color = float4(color, constants.debug_opacity);
    return output;
}
