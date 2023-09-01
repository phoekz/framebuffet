#pragma once

#include <kitchen/gpu/hlsl_cpp.hlsli>

FB_NAMESPACE_BEGIN(fb::demos::rain)

FB_CONSTANT uint DISPATCH_X = 128;
FB_CONSTANT uint DISPATCH_Y = 1;
FB_CONSTANT uint DISPATCH_Z = 1;

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

struct Vertex {
    float3 position;
    float2 texcoord;
};

struct Particle {
    float3 position;
};

FB_NAMESPACE_END(fb::demos::rain)
