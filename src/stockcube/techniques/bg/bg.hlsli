#pragma once

#include <kitchen/gpu/hlsl_cpp.hlsli>

FB_NAMESPACE_BEGIN(fb::techniques::bg)

struct Bindings {
    uint constants;
    uint vertices;
    uint texture;
    uint sampler;
};

struct Constants {
    float4x4 transform;
    float roughness;
    uint mip_count;
    float pad[46];
};

FB_NAMESPACE_END(fb::techniques::bg)
