#pragma once

#include <gpu/hlsl_cpp.hlsli>

FB_NAMESPACE_BEGIN(fb::demos::anim)

struct Bindings {
    uint constants;
    uint vertices;
    uint joints_inverse_binds;
    uint joints_global_transforms;
    uint texture;
};

struct Constants {
    float4x4 transform;
    float pad[48];
};

FB_NAMESPACE_END(fb::demos::anim)
