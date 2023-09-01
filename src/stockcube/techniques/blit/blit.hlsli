#pragma once

#include <kitchen/gpu/hlsl_cpp.hlsli>

FB_NAMESPACE_BEGIN(fb::techniques::blit)

struct Bindings {
    uint constants;
    uint texture;
};

struct Constants {
    uint tonemap;
    float exposure;
    float pad[62];
};

FB_NAMESPACE_END(fb::techniques::blit)
