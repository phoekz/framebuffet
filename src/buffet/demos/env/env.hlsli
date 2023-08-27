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

struct ComputeBindings {
    uint constants;
    uint rect_texture;
    uint cube_texture;
    uint lut_texture;
};

struct ComputeConstants {
    float2 rect_texture_size;
    float2 cube_texture_size;
    float2 lut_texture_size;
    float pad[58];
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
    float pad[47];
};

//
// Screen.
//

struct ScreenBindings {
    uint constants;
    uint vertices;
    uint texture;
    uint texture_slice;
    float2 screen_offset;
    float2 screen_scale;
};

struct ScreenConstants {
    float4x4 transform;
    uint tonemap;
    float pad[47];
};

struct ScreenVertex {
    float2 position;
    float2 texcoord;
};

FB_NAMESPACE_END(fb::demos::env)
