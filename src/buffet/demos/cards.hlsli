#pragma once

#include <kitchen/gpu/hlsl_cpp.hlsli>

FB_NAMESPACE_BEGIN(fb::demos::cards)

struct Bindings {
    uint card_index;
    uint constants;
    uint cards;
    uint vertices;
    uint texture;
};

struct Constants {
    float4x4 transform;
    float pad[48];
};

struct Card {
    float2 position;
    float2 size;
};

struct Vertex {
    float2 position;
    float2 texcoord;
};

FB_NAMESPACE_END(fb::demos::cards)
