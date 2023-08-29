#pragma once

#include <kitchen/gpu/hlsl_cpp.hlsli>

FB_NAMESPACE_BEGIN(fb::demos::crate)

struct Bindings {
    uint constants;
    uint vertices;
    uint base_color_texture;
    uint normal_texture;
    uint metallic_roughness_texture;
    uint sampler;
};

enum class OutputMode : uint {
    Shaded = 0,
    ShadingNormal,
    Lighting,
    VertexLighting,
    BaseColorTexture,
    NormalTexture,
    Metallic,
    Roughness,
    VertexTexCoord,
    VertexNormal,
    VertexTangent,
    VertexBitangent,
    LevelOfDetail,
    DirectionToViewer,
    DirectionToLight,
};

struct Constants {
    float4x4 transform;
    float3 light_direction;
    float light_ambient;
    float light_intensity;
    float3 camera_position;
    OutputMode output_mode;
    float pad[39];
};

FB_NAMESPACE_END(fb::demos::crate)
