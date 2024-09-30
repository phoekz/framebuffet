#pragma once

#include <kitchen/gpu/hlsl_cpp.hlsli>

FB_NAMESPACE_BEGIN(fb::demos::anim)

struct Bindings {
    uint constants;
    uint vertices;
    uint skinning_matrices;
    uint color;
    uint texture;
    float texture_scroll;
    uint shadow_texture;
};

struct Constants {
    float4x4 transform;
    float4x4 light_transform;
    float pad[32];
};

struct Instance {
    float4x4 transform;
};

FB_NAMESPACE_END(fb::demos::anim)
