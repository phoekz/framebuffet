#pragma once

#include <gpu/hlsl_cpp.hlsli>

FB_NAMESPACE_BEGIN(fb::demos::tree)

struct Bindings {
    uint constants;
    uint vertices;
    uint texture;
    uint shadow_texture;
};

FB_NAMESPACE_END(fb::demos::tree)
