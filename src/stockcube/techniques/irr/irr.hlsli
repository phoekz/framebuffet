#pragma once

#include <kitchen/gpu/hlsl_cpp.hlsli>

FB_NAMESPACE_BEGIN(fb::techniques::irr)

FB_CONSTANT uint DISPATCH_X = 8;
FB_CONSTANT uint DISPATCH_Y = 8;
FB_CONSTANT uint DISPATCH_Z = 1;

struct AccBindings {
    uint dispatch_id;
    uint constants;
    uint cube_texture;
    uint cube_sampler;
    uint irr_texture;
};

struct DivBindings {
    uint dispatch_id;
    uint constants;
    uint acc_texture;
    uint div_texture;
};

struct Constants {
    uint2 irr_texture_size;
    uint irr_sample_count;
    uint irr_sample_count_per_dispatch;
    float pad[60];
};

FB_NAMESPACE_END(fb::techniques::irr)
