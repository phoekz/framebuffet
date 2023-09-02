#pragma once

#include <kitchen/gpu/hlsl_cpp.hlsli>

FB_NAMESPACE_BEGIN(fb::demos::text)

struct BackgroundBindings {
    uint constants;
    uint vertices;
    uint irr_texture;
    uint sampler;
};

struct BackgroundConstants {
    float4x4 transform;
    float pad[48];
};

struct GlyphBindings {
    uint constants;
    uint vertices;
    uint instances;
    uint irr_texture;
    uint sampler;
    uint base_vertex;
    uint instance_id;
};

struct GlyphConstants {
    float4x4 transform;
    float4x4 scene_transform;
    float4 color;
    float pad[28];
};

struct GlyphInstance {
    float3 position;
};

FB_NAMESPACE_END(fb::demos::text)
