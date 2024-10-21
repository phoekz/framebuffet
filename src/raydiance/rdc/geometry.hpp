#pragma once

#include "rdc.hpp"

namespace fb {

//
// Common.
//

inline constexpr uint RDC_NIL = 0xffffffff;

//
// Ray.
//

struct RdcRay {
    float3 origin = {};
    float3 dir = {};
};

auto rdc_ray_from_sensor(
    const uint2& image_size,
    const uint2& pixel_idx,
    const float3& camera_position,
    const float4x4& world_from_clip,
    float s,
    float t
) -> RdcRay;

//
// Triangle.
//

// Todo: consider splitting into positions and attributes, with index buffer.
struct RdcTriangle {
    std::array<float3, 3> positions = {};
    std::array<float2, 3> texcoords = {};
    std::array<float3, 3> normals = {};
    uint material = {};
};

//
// Axis-aligned bounding box.
//

union RdcAabb {
    std::array<float3, 2> extents = {};

    struct {
        float3 min;
        float3 max;
    };
};

FB_INLINE constexpr auto rdc_aabb_new() -> RdcAabb {
    return RdcAabb {
        .min = float3(FLT_MAX),
        .max = float3(-FLT_MAX),
    };
}

FB_INLINE constexpr auto rdc_aabb_center(const RdcAabb& aabb) -> float3 {
    float3 center;
    center.x = 0.5f * (aabb.min.x + aabb.max.x);
    center.y = 0.5f * (aabb.min.y + aabb.max.y);
    center.z = 0.5f * (aabb.min.z + aabb.max.z);
    return center;
}

FB_INLINE constexpr auto rdc_aabb_extents(const RdcAabb& aabb) -> float3 {
    float3 extents;
    extents.x = aabb.max.x - aabb.min.x;
    extents.y = aabb.max.y - aabb.min.y;
    extents.z = aabb.max.z - aabb.min.z;
    return extents;
}

FB_INLINE constexpr auto rdc_aabb_surface_area(const RdcAabb& aabb) -> float {
    const float3 extents = rdc_aabb_extents(aabb);
    return 2.0f * (extents.x * extents.y + extents.x * extents.z + extents.y * extents.z);
}

FB_INLINE constexpr auto rdc_aabb_extended(const RdcAabb& aabb, const float3& point) -> RdcAabb {
    RdcAabb extended;
    extended.min.x = std::min(aabb.min.x, point.x);
    extended.min.y = std::min(aabb.min.y, point.y);
    extended.min.z = std::min(aabb.min.z, point.z);
    extended.max.x = std::max(aabb.max.x, point.x);
    extended.max.y = std::max(aabb.max.y, point.y);
    extended.max.z = std::max(aabb.max.z, point.z);
    return extended;
}

FB_INLINE constexpr auto rdc_aabb_merged(const RdcAabb& aabb, const RdcAabb& other) -> RdcAabb {
    RdcAabb merged;
    merged.min.x = std::min(aabb.min.x, other.min.x);
    merged.min.y = std::min(aabb.min.y, other.min.y);
    merged.min.z = std::min(aabb.min.z, other.min.z);
    merged.max.x = std::max(aabb.max.x, other.max.x);
    merged.max.y = std::max(aabb.max.y, other.max.y);
    merged.max.z = std::max(aabb.max.z, other.max.z);
    return merged;
}

//
// Bounding volume hierarchy.
//

struct RdcBvhNode {
    RdcAabb bounds;
    uint offset;
    uint8_t primitive_count;
    uint8_t axis;
    uint16_t pad;
};

auto rdc_bvh_create(std::vector<RdcBvhNode>& nodes, std::vector<RdcTriangle>& triangles) -> void;
auto rdc_bvh_hit(
    const RdcRay& ray,
    std::span<const RdcBvhNode> nodes,
    std::span<const RdcTriangle> triangles,
    float& out_t,
    float3& out_uvw,
    uint& out_triangle_idx
) -> bool;

//
// Ray-triangle intersector.
//

struct RdcRayTriangleIntersector {
    uint3 k = {};
    float3 s = {};
};

auto rdc_ray_triangle_intersector_create(const RdcRay& ray) -> RdcRayTriangleIntersector;
auto rdc_ray_triangle_intersector_hit(
    const RdcRayTriangleIntersector& intersector,
    const RdcRay& ray,
    const RdcTriangle& triangle,
    float& out_t,
    float3& out_uvw
) -> bool;

//
// Ray-aabb intersector.
//

struct RdcRayAabbIntersector {
    float3 ray_dir_inv = {};
    std::array<uint, 3> ray_dir_neg = {};
};

auto rdc_ray_aabb_intersector_create(const RdcRay& ray) -> RdcRayAabbIntersector;
auto rdc_ray_aabb_intersector_hit(
    const RdcRayAabbIntersector& intersector,
    const RdcRay& ray,
    const RdcAabb& aabb,
    float best_t
) -> bool;

//
// Basis.
//

struct RdcOrthonormalBasis {
    float3x3 world_from_local;
    float3x3 local_from_world;
};

auto rdc_orthonormal_basis_from_normal(const float3& normal) -> RdcOrthonormalBasis;

//
// Debug.
//

auto rdc_bruteforce_hit(
    const RdcRay& ray,
    std::span<const RdcTriangle> triangles,
    float& out_t,
    float3& out_uvw,
    uint& out_triangle_idx
) -> bool;

} // namespace fb
