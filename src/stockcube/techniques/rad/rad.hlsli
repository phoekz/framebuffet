#pragma once

#include <kitchen/gpu/hlsl_cpp.hlsli>

FB_NAMESPACE_BEGIN(fb::techniques::rad)

FB_CONSTANT uint DISPATCH_X = 8;
FB_CONSTANT uint DISPATCH_Y = 8;
FB_CONSTANT uint DISPATCH_Z = 1;

struct AccBindings {
    uint dispatch_id;
    uint constants;
    uint cube_texture;
    uint cube_sampler;
    uint rad_texture;
    uint rad_texture_mip_id;
};

struct DivBindings {
    uint dispatch_id;
    uint mip_id;
    uint constants;
    uint acc_texture;
    uint div_texture;
};

struct Constants {
    uint2 rad_texture_size;
    uint rad_texture_mip_count;
    uint rad_sample_count;
    uint rad_sample_count_per_dispatch;
    float pad[59];
};

FB_NAMESPACE_END(fb::techniques::rad)
