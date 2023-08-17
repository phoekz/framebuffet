#pragma once

#include "hlsl_cpp.hlsli"

FB_NAMESPACE_BEGIN(fb::fibers)

FB_CONSTANT uint LIGHT_COUNT = 32;
FB_CONSTANT uint LIGHT_CAPACITY_PER_TILE = 8;
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

FB_NAMESPACE_END(fb::fibers)
