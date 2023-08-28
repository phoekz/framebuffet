#pragma once

#include <gpu/hlsl_cpp.hlsli>

FB_NAMESPACE_BEGIN(fb::graphics::gui)

struct Bindings {
    uint constants;
    uint vertices;
    uint base_vertex;
    uint texture;
};

struct Constants {
    float4x4 transform;
    float pad[48];
};

FB_NAMESPACE_END(fb::graphics::gui)
