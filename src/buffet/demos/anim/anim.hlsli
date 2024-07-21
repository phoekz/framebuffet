#pragma once

#include <kitchen/gpu/hlsl_cpp.hlsli>

FB_NAMESPACE_BEGIN(fb::demos::anim)

struct Bindings {
    uint constants;
    uint vertices;
    uint skinning_matrices;
    uint color;
};

struct Constants {
    float4x4 transform;
    float pad[48];
};

struct Instance {
    float4x4 transform;
};

FB_NAMESPACE_END(fb::demos::anim)
