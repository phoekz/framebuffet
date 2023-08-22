#pragma once

#include <gpu/hlsl_cpp.hlsli>

FB_NAMESPACE_BEGIN(fb::demos::cube)

struct Bindings {
    uint constants;
    uint vertices;
    uint texture;
};

enum class OutputMode : uint {
    Shaded = 0,
    Lighting,
    BaseColor,
    TexCoord,
    VertexNormal,
    VertexTangent,
    VertexBitangent,
};

struct Constants {
    float4x4 transform;
    float3 light_direction;
    float light_ambient;
    OutputMode output_mode;
    float pad[43];
};

FB_NAMESPACE_END(fb::demos::cube)
