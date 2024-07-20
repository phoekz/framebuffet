#pragma once

#include <kitchen/gpu/hlsl_cpp.hlsli>

FB_NAMESPACE_BEGIN(fb::demos::cards)

struct BackgroundBindings {
    uint constants;
    uint texture;
};

struct DrawBindings {
    uint card_index;
    uint constants;
    uint cards;
    uint vertices;
    uint texture;
};

struct Constants {
    float4x4 transform;
    float zoom_factor;
    float lod_bias;
    float pad[46];
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
