#pragma once

#include <kitchen/gpu/hlsl_cpp.hlsli>

FB_NAMESPACE_BEGIN(fb::techniques::cfr)

FB_CONSTANT uint DISPATCH_X = 8;
FB_CONSTANT uint DISPATCH_Y = 8;
FB_CONSTANT uint DISPATCH_Z = 1;

struct Bindings {
    uint constants;
    uint rect_texture;
    uint rect_sampler;
    uint cube_texture;
};

struct Constants {
    uint2 cube_texture_size;
    float pad[62];
};

FB_NAMESPACE_END(fb::techniques::cfr)
