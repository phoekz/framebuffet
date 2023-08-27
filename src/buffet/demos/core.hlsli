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

struct FbPixelOutput1 {
    float4 color: SV_Target;
};

struct FbPixelOutput2 {
    float4 color_0: SV_Target0;
    float4 color_1: SV_Target1;
};

struct FbPixelOutput3 {
    float4 color_0: SV_Target0;
    float4 color_1: SV_Target1;
    float4 color_2: SV_Target2;
};

struct FbPixelOutput4 {
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
    texcoord = float2((vertex_id << 1) & 2, vertex_id & 2);
    position = float4(texcoord * float2(2.0f, -2.0f) + float2(-1.0f, 1.0f), 0.0f, 1.0f);
}

float2 fb_screen_coord_from_ndc(float4 ndc) {
    // Note: Y is flipped.
    return ndc.xy / ndc.w * float2(0.5f, -0.5f) + float2(0.5f, 0.5f);
}

float3 linear_from_srgb(float3 srgb) {
    // https://chilliant.blogspot.com/2012/08/srgb-approximations-for-hlsl.html
    return srgb * (srgb * (srgb * 0.305306011f + 0.682171111f) + 0.012522878f);
}
