#pragma once

#include <gpu/hlsl_cpp.hlsli>

FB_NAMESPACE_BEGIN(fb::demos::gui)

struct Bindings {
    uint constants;
    uint vertices;
    uint base_vertex;
    uint texture;
};

FB_NAMESPACE_END(fb::demos::gui)
