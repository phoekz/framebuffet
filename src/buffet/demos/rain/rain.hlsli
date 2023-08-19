#pragma once

#include <gpu/hlsl_cpp.hlsli>

FB_NAMESPACE_BEGIN(fb::demos::rain)

FB_CONSTANT uint SIM_DISPATCH_SIZE = 128;

struct Bindings {
    uint constants;
    uint particles;
    uint vertices;
};

struct Constants {
    float4x4 transform;
    float4x4 particle_transform;
    float delta_time;
    float speed;
    float pad[30];
};

FB_NAMESPACE_END(fb::demos::rain)
