#pragma once

#include <kitchen/gpu/hlsl_cpp.hlsli>

FB_NAMESPACE_BEGIN(fb::screen)

struct Bindings {
    uint constants;
    uint vertices;
    uint instances;
};

struct Constants {
    float4x4 transform;
    float pad[48];
};

struct Vertex {
    float2 position;
    float2 texcoord;
};

struct Instance {
    float2 offset;
    float2 scale;
    uint texture;
};

FB_NAMESPACE_END(fb::screen)
