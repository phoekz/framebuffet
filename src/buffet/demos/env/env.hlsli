#pragma once

#include <gpu/hlsl_cpp.hlsli>

FB_NAMESPACE_BEGIN(fb::demos::env)

//
// Compute.
//

FB_CONSTANT uint CFR_DISPATCH_X = 8;
FB_CONSTANT uint CFR_DISPATCH_Y = 8;
FB_CONSTANT uint CFR_DISPATCH_Z = 1;

FB_CONSTANT uint LUT_DISPATCH_X = 8;
FB_CONSTANT uint LUT_DISPATCH_Y = 8;
FB_CONSTANT uint LUT_DISPATCH_Z = 1;

FB_CONSTANT uint IRR_DISPATCH_X = 8;
FB_CONSTANT uint IRR_DISPATCH_Y = 8;
FB_CONSTANT uint IRR_DISPATCH_Z = 1;

FB_CONSTANT uint RAD_DISPATCH_X = 8;
FB_CONSTANT uint RAD_DISPATCH_Y = 8;
FB_CONSTANT uint RAD_DISPATCH_Z = 1;

struct ComputeBindings {
    uint constants;
    uint rect_texture;
    uint cube_texture;
    uint lut_texture;
    uint irr_texture;
    uint irr_dispatch_index;
    uint rad_texture;
    uint rad_mip_id;
    uint rad_dispatch_index;
};

struct ComputeConstants {
    uint2 rect_texture_size;
    uint2 cube_texture_size;

    uint2 lut_texture_size;
    uint lut_sample_count;
    uint lut_pad;

    uint2 irr_texture_size;
    uint irr_sample_count;
    uint irr_dispatch_sample_count;

    uint2 rad_texture_size;
    uint rad_texture_mip_count;
    uint rad_sample_count;

    uint rad_dispatch_sample_count;
    uint rad_pad0, rad_pad1, rad_pad2;

    float pad[44];
};

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

//
// Screen.
//

struct ScreenBindings {
    uint constants;
    uint vertices;
    uint texture;
    uint texture_face_id;

    uint texture_mip_id;
    uint pad0, pad1, pad2;

    float2 screen_offset;
    float2 screen_scale;
};

struct ScreenConstants {
    float4x4 transform;
    uint tonemap;
    float exposure;
    float pad[46];
};

struct ScreenVertex {
    float2 position;
    float2 texcoord;
};

FB_NAMESPACE_END(fb::demos::env)
