#pragma once

#include <kitchen/gpu/hlsl_cpp.hlsli>

FB_NAMESPACE_BEGIN(fb::demos::grass)

struct Bindings {
    uint constants;
    uint vertices;
    uint texture;
};

struct Constants {
    float4x4 transform;
    float alpha_cutoff;
    float pad[47];
};

FB_NAMESPACE_END(fb::demos::grass)
