#pragma once

#include <kitchen/gpu/hlsl_cpp.hlsli>

FB_NAMESPACE_BEGIN(fb::techniques::lut)

FB_CONSTANT uint DISPATCH_X = 8;
FB_CONSTANT uint DISPATCH_Y = 8;
FB_CONSTANT uint DISPATCH_Z = 1;

struct Bindings {
    uint constants;
    uint lut_texture;
};

struct Constants {
    uint2 lut_texture_size;
    uint lut_sample_count;
    float pad[61];
};

FB_NAMESPACE_END(fb::techniques::lut)
