#pragma once

#include <kitchen/gpu/hlsl_cpp.hlsli>

FB_NAMESPACE_BEGIN(fb::demos::conras)

enum class ConservativeRasterization : uint {
    Off = 0,
    Over,
    Under,
    OverUnder,
};

struct Bindings {
    uint constants;
    uint raster_buffer;
    ConservativeRasterization conservative_rasterization;
};

struct Constants {
    float4x4 transform;
    uint2 raster_buffer_size;
    float pad[46];
};

FB_CONSTANT float3 STAR_VERTICES[11] = {
    float3(0.0f, 0.0f, 0.0f),
    float3(0.0f, 0.5f, 0.0f),
    float3(-0.118f, 0.162f, 0.0f),
    float3(-0.476f, 0.155f, 0.0f),
    float3(-0.19f, -0.0618f, 0.0f),
    float3(-0.294f, -0.405f, 0.0f),
    float3(0.0f, -0.2f, 0.0f),
    float3(0.294f, -0.405f, 0.0f),
    float3(0.19f, -0.0618f, 0.0f),
    float3(0.476f, 0.155f, 0.0f),
    float3(0.118f, 0.162f, 0.0f),
};
FB_CONSTANT uint STAR_TRIANGLE_COUNT = 10;
FB_CONSTANT uint16_t STAR_INDICES[3 * STAR_TRIANGLE_COUNT] = {
    0, 1,  2,  //
    0, 2,  3,  //
    0, 3,  4,  //
    0, 4,  5,  //
    0, 5,  6,  //
    0, 6,  7,  //
    0, 7,  8,  //
    0, 8,  9,  //
    0, 9,  10, //
    0, 10, 1,  //
};

FB_NAMESPACE_END(fb::demos::conras)
