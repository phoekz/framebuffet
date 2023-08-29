#pragma once

#include <kitchen/gpu/hlsl_cpp.hlsli>

FB_NAMESPACE_BEGIN(fb::demos::fibers)

FB_CONSTANT uint LIGHT_COUNT = 32;
FB_CONSTANT uint MAX_LIGHT_PER_TILE = 8;
FB_CONSTANT uint SIM_DISPATCH_SIZE = 32;
FB_CONSTANT uint CULL_TILE_SIZE = 16;
FB_CONSTANT uint CULL_DISPATCH_SIZE = 16;
FB_CONSTANT float PLANE_RADIUS_X = 2.0f;
FB_CONSTANT float PLANE_RADIUS_Y = 1.0f;

struct Bindings {
    uint constants;
    uint lights;
    uint vertices;
    uint heatmap_texture;
    uint light_counts_texture;
    uint light_offsets_texture;
    uint light_indices;
    uint light_indices_count;
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
    float heatmap_opacity;
    float pad[9];
};

struct Light {
    float3 position;
    float3 color;
    float speed_variation;
};

FB_NAMESPACE_END(fb::demos::fibers)
