#pragma once

#include <gpu/hlsl_cpp.hlsli>

FB_NAMESPACE_BEGIN(fb::demos::rain)

FB_CONSTANT uint SIM_DISPATCH_SIZE = 128;

struct Bindings {
    uint constants;
    uint particles;
    uint vertices;
};

FB_NAMESPACE_END(fb::demos::rain)
