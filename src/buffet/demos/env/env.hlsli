#pragma once

#include <gpu/hlsl_cpp.hlsli>

FB_NAMESPACE_BEGIN(fb::demos::env)

struct Bindings {
    uint constants;
    uint vertices;
    uint texture;
};

FB_NAMESPACE_END(fb::demos::env)
