#pragma once

#include <kitchen/gpu/hlsl_cpp.hlsli>

FB_NAMESPACE_BEGIN(fb::kcn::spd)

struct Bindings {
    uint constants;
    uint atomics;
    uint texture_src;
    uint texture_mid;
    uint texture_dst_begin;
};

struct Constants {
    uint mip_count;
    uint threadgroup_count;
    float2 inv_texture_size;
    float pad[60];
};

struct Atomics {
    uint counter[6];
};

FB_NAMESPACE_END(fb::kcn::spd)
