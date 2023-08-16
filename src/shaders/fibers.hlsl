#include "shaders/core.hlsli"
#include "shaders/samplers.hlsli"
#include "shaders/fibers.hlsli"

struct Bindings {
    uint constants;
    uint lights;
    uint vertices;
    uint cull_texture;
    uint heatmap_texture;
};

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

groupshared float4 gs_frustum_planes[4];
groupshared uint gs_hit_count;

float3 view_point_from_clip(float4x4 view_from_clip, float clip_x, float clip_y) {
    const float4 clip = float4(clip_x, clip_y, -1.0f, 1.0f);
    const float4 view = mul(view_from_clip, clip);
    return view.xyz / view.w;
}

FB_ATTRIBUTE(numthreads, CULL_DISPATCH_SIZE, CULL_DISPATCH_SIZE, 1)
void cull_cs(FbComputeInput input) {
    ConstantBuffer<Constants> constants = ResourceDescriptorHeap[g_bindings.constants];
    StructuredBuffer<Light> lights = ResourceDescriptorHeap[g_bindings.lights];
    RWTexture2D<uint> cull_texture = ResourceDescriptorHeap[g_bindings.cull_texture];

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

        gs_hit_count = 0;
    }
    GroupMemoryBarrierWithGroupSync();

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
            InterlockedAdd(gs_hit_count, 1);
        }
    }
    GroupMemoryBarrierWithGroupSync();

    // Write.
    if (input.group_index == 0) {
        cull_texture[uint2(input.group_id.xy)] = gs_hit_count;
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
    return output;
}

FbPixelOutput1 plane_ps(PlaneVertexOutput input) {
    ConstantBuffer<Constants> constants = ResourceDescriptorHeap[g_bindings.constants];
    StructuredBuffer<Light> lights = ResourceDescriptorHeap[g_bindings.lights];

    float3 color = float3(0.02f, 0.02f, 0.02f);

    for (uint i = 0; i < LIGHT_COUNT; i++) {
        Light light = lights[i];

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
    RWTexture2D<uint> cull_texture = ResourceDescriptorHeap[g_bindings.cull_texture];
    Texture2D<float3> heatmap_texture = ResourceDescriptorHeap[g_bindings.heatmap_texture];
    SamplerState heatmap_sampler = SamplerDescriptorHeap[(uint)GpuSamplerType::LinearClamp];

    const float2 window_size = constants.window_size;
    const float2 tile_size = (float2)CULL_TILE_SIZE;
    const float2 tile_pixel = input.texcoord * window_size / tile_size;
    const uint2 tile_index = floor(tile_pixel);
    uint light_count = cull_texture[tile_index];
    float shade = saturate((float)light_count / 5.0f);
    float3 color = heatmap_texture.Sample(heatmap_sampler, float2(shade, 0.5f));

    FbPixelOutput1 output;
    output.color = float4(color, constants.debug_opacity);
    return output;
}
