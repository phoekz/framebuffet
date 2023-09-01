#pragma once

#include <kitchen/gpu/hlsl_cpp.hlsli>

FB_NAMESPACE_BEGIN(fb::demos::env)

//
// Background.
//

struct BackgroundBindings {
    uint constants;
    uint vertices;
    uint texture;
};

struct BackgroundConstants {
    float4x4 transform;
    uint tonemap;
    float exposure;
    float roughness;
    uint mip_count;
    float pad[44];
};

//
// Model.
//

struct ModelBindings {
    uint constants;
    uint vertices;
    uint lut_texture;
    uint irr_texture;
    uint rad_texture;
};

struct ModelConstants {
    float4x4 transform;
    float3 camera_position;
    uint tonemap;
    float metallic;
    float roughness;
    uint mip_count;
    float pad[41];
};

FB_NAMESPACE_END(fb::demos::env)
