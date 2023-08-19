#pragma once

#include <gpu/hlsl_cpp.hlsli>

FB_NAMESPACE_BEGIN(fb::demos::cube)

struct Bindings {
    uint constants;
    uint vertices;
    uint texture;
};

struct Constants {
    float4x4 transform;
    float pad[48];
};

FB_NAMESPACE_END(fb::demos::cube)
