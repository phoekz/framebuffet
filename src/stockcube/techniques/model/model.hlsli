#pragma once

#include <kitchen/gpu/hlsl_cpp.hlsli>

FB_NAMESPACE_BEGIN(fb::techniques::model)

FB_CONSTANT uint DISPATCH_X = 1; // Todo.
FB_CONSTANT uint DISPATCH_Y = 1; // Todo.
FB_CONSTANT uint DISPATCH_Z = 1; // Todo.

struct Bindings {
    uint constants;
    uint vertices;
    uint lut_texture;
    uint irr_texture;
    uint rad_texture;
};

struct Constants {
    float4x4 transform;
    float3 camera_position;
    float metallic;
    float roughness;
    uint mip_count;
    float pad[42];
};

FB_NAMESPACE_END(fb::techniques::model)
