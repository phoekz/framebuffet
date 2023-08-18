#pragma once

#include <gpu/hlsl_cpp.hlsli>

FB_NAMESPACE_BEGIN(fb::demos::cards)

struct Bindings {
    uint card_index;
    uint constants;
    uint cards;
    uint vertices;
    uint texture;
};

FB_NAMESPACE_END(fb::demos::cards)
