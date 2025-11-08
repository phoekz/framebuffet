#pragma once

#include <kitchen/gpu/hlsl_cpp.hlsli>

FB_NAMESPACE_BEGIN(fb::griddle)

struct Vertex {
    float2 position;
    float2 texcoord;
};

struct Bindings {
    uint constants;
    uint vertices;
    uint texture;
    uint sampler;
};

struct Constants {
    float4x4 transform;
    uint pad[48];
};

FB_NAMESPACE_END(fb::griddle)
