#pragma once

//
// Constants.
//

static const float FB_PI = 3.141592654f;
static const float FB_TWO_PI = 6.283185307f;
static const float FB_ONE_OVER_PI = 0.318309886f;

//
// Macros.
//

#define FB_ATTRIBUTE(attribute, ...) [attribute(__VA_ARGS__)]

//
// Input/output types.
//

struct FbVertexInput {
    uint vertex_id: SV_VertexID;
    uint instance_id: SV_InstanceID;
};

template<uint TargetCount>
struct FbPixelOutput {};

template<>
struct FbPixelOutput<1> {
    float4 color: SV_Target;
};

template<>
struct FbPixelOutput<2> {
    float4 color_0: SV_Target0;
    float4 color_1: SV_Target1;
};

template<>
struct FbPixelOutput<3> {
    float4 color_0: SV_Target0;
    float4 color_1: SV_Target1;
    float4 color_2: SV_Target2;
};

template<>
struct FbPixelOutput<4> {
    float4 color_0: SV_Target0;
    float4 color_1: SV_Target1;
    float4 color_2: SV_Target2;
    float4 color_3: SV_Target3;
};

struct FbComputeInput {
    uint3 group_thread_id: SV_GroupThreadID;
    uint3 group_id: SV_GroupID;
    uint3 dispatch_thread_id: SV_DispatchThreadID;
    uint group_index: SV_GroupIndex;
};

//
// Mesh.
//

struct FbVertex {
    float3 position;
    float3 normal;
    float2 texcoord;
    float4 tangent;
};

struct FbSkinningVertex {
    float3 position;
    float3 normal;
    float2 texcoord;
    float4 tangent;
    uint4 joints;
    float4 weights;
};

//
// Utilities.
//

float fb_pow2(float x) {
    return x * x;
}

float fb_pow4(float x) {
    float x2 = x * x;
    return x2 * x2;
}

float4 fb_plane_from_points(float3 p0, float3 p1, float3 p2) {
    float3 v0 = p1 - p0;
    float3 v1 = p2 - p0;
    float3 n = normalize(cross(v0, v1));
    return float4(n, dot(n, p0));
}

bool fb_sphere_inside_plane(float4 plane, float3 sphere_center, float sphere_radius) {
    float d = dot(plane.xyz, sphere_center) - plane.w;
    return d < -sphere_radius;
}

void fb_fullscreen_triangle(uint vertex_id, out float4 position, out float2 texcoord) {
    // Note: counter-clockwise winding.
    texcoord = float2(vertex_id & 2, (vertex_id << 1) & 2);
    position = float4(texcoord * float2(2.0f, -2.0f) + float2(-1.0f, 1.0f), 0.0f, 1.0f);
}

float2 fb_screen_coord_from_ndc(float4 ndc) {
    // Note: Y is flipped.
    return ndc.xy / ndc.w * float2(0.5f, -0.5f) + float2(0.5f, 0.5f);
}

float3 fb_linear_from_srgb(float3 srgb) {
    // From: https://chilliant.blogspot.com/2012/08/srgb-approximations-for-hlsl.html
    return srgb * (srgb * (srgb * 0.305306011f + 0.682171111f) + 0.012522878f);
}

float3 fb_cube_direction_from_dispatch_input(uint2 src_id, uint face_id, uint2 face_size) {
    // Note: Y is flipped.
    const float2 p = float2(
        (2.0f * ((src_id.x + 0.5f) / face_size.x) - 1.0f),
        -(2.0f * ((src_id.y + 0.5f) / face_size.y) - 1.0f)
    );

    float3 dir;
    switch (face_id) {
        case 0: dir = float3(1.0f, p.y, -p.x); break;
        case 1: dir = float3(-1.0f, p.y, p.x); break;
        case 2: dir = float3(p.x, 1.0f, -p.y); break;
        case 3: dir = float3(p.x, -1.0f, p.y); break;
        case 4: dir = float3(p.x, p.y, 1.0); break;
        case 5: dir = float3(-p.x, p.y, -1.0); break;
    }
    dir = normalize(dir);
    return dir;
}

float2 fb_hammersley2d(uint i, uint n) {
    // From: http://holger.dammertz.org/stuff/notes_HammersleyOnHemisphere.html
    uint bits = (i << 16u) | (i >> 16u);
    bits = ((bits & 0x55555555u) << 1u) | ((bits & 0xaaaaaaaau) >> 1u);
    bits = ((bits & 0x33333333u) << 2u) | ((bits & 0xccccccccu) >> 2u);
    bits = ((bits & 0x0f0f0f0fu) << 4u) | ((bits & 0xf0f0f0f0u) >> 4u);
    bits = ((bits & 0x00ff00ffu) << 8u) | ((bits & 0xff00ff00u) >> 8u);
    return float2(float(i) / float(n), float(bits) * 2.3283064365386963e-10f);
}

float3 fb_tonemap_aces(float3 x) {
    // From: https://knarkowicz.wordpress.com/2016/01/06/aces-filmic-tone-mapping-curve/
    const float a = 2.51f;
    const float b = 0.03f;
    const float c = 2.43f;
    const float d = 0.59f;
    const float e = 0.14f;
    return saturate((x * (a * x + b)) / (x * (c * x + d) + e));
}
