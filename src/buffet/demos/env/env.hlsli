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
FB_CONSTANT uint IRR_SAMPLE_COUNT = (1 << 16);
FB_CONSTANT uint IRR_DISPATCH_SAMPLE_COUNT = 256;

struct ComputeBindings {
    uint constants;
    uint rect_texture;
    uint cube_texture;
    uint lut_texture;
    uint irr_texture;
    uint irr_dispatch_index;
};

struct ComputeConstants {
    float2 rect_texture_size;
    float2 cube_texture_size;
    float2 lut_texture_size;
    float2 irr_texture_size;
    float pad[56];
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
    float pad[46];
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
    float exposure;
    float pad[46];
};

struct ScreenVertex {
    float2 position;
    float2 texcoord;
};

FB_NAMESPACE_END(fb::demos::env)
