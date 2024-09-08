#pragma once

#include <kitchen/gpu/hlsl_cpp.hlsli>

FB_NAMESPACE_BEGIN(fb::kcn::debug_draw)

struct Bindings {
    uint constants;
    uint vertices;
};

FB_NAMESPACE_END(fb::kcn::debug_draw)
