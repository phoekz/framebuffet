#pragma once

#include <kitchen/gpu/hlsl_cpp.hlsli>

FB_NAMESPACE_BEGIN(fb::kcn::gui)

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

FB_NAMESPACE_END(fb::kcn::gui)
