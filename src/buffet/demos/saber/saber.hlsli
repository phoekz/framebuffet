#pragma once

#include <kitchen/gpu/hlsl_cpp.hlsli>

FB_NAMESPACE_BEGIN(fb::demos::saber)

FB_CONSTANT uint DISPATCH_X = 8;
FB_CONSTANT uint DISPATCH_Y = 8;
FB_CONSTANT uint DISPATCH_Z = 1;

FB_CONSTANT float THRESHOLD = 1.0f;

struct SceneBindings {
    uint constants;
    uint vertices;
    uint instances;
};

struct SceneInstance {
    float4x4 transform;
};

struct ThresholdBindings {
    uint constants;
    uint scene_texture;
    uint downsample_texture;
    uint texture_width;
    uint texture_height;
};

struct DownsampleBindings {
    uint constants;

    uint src_texture;
    uint src_texture_level;
    uint src_texture_width;
    uint src_texture_height;

    uint dst_texture;
    uint dst_texture_level;
    uint dst_texture_width;
    uint dst_texture_height;
};

struct UpsampleBindings {
    uint constants;

    uint src_dn_texture;
    uint src_up_texture;
    uint src_texture_level;
    uint src_texture_width;
    uint src_texture_height;

    uint dst_texture;
    uint dst_texture_level;
    uint dst_texture_width;
    uint dst_texture_height;
};

struct BlitBindings {
    uint constants;
    uint scene_texture;
    uint bloom_texture;
};

struct Constants {
    float4x4 transform;
    float4 saber_color_and_intensity;
    uint tonemap;
    float pad[43];
};

FB_NAMESPACE_END(fb::demos::saber)
