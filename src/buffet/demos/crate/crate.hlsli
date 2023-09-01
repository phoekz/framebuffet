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
    uint lut_texture;
    uint irr_texture;
    uint rad_texture;
};

enum class OutputMode : uint {
    Shaded = 0,
    EnvDiffuse,
    EnvSpecular,
    EnvLut,
    EnvIrradiance,
    EnvRadiance,
    DirectLighting,
    DirectBrdf,
    VertexLighting,
    ShadingNormal,
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
    uint rad_texture_mip_count;
    float pad[38];
};

FB_NAMESPACE_END(fb::demos::crate)
