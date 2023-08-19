#pragma once

#include <gpu/hlsl_cpp.hlsli>

FB_NAMESPACE_BEGIN(fb::demos::tree)

struct Bindings {
    uint constants;
    uint vertices;
    uint texture;
    uint shadow_texture;
};

struct Constants {
    float4x4 transform;
    float4x4 light_transform;
    float3 light_direction;
    float ambient_light;
    float pad[28];
};

FB_NAMESPACE_END(fb::demos::tree)
