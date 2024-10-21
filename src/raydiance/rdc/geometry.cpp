#include "geometry.hpp"

namespace fb {

//
// Ray.
//

auto rdc_ray_from_sensor(
    const uint2& image_size,
    const uint2& pixel_idx,
    const float3& camera_position,
    const float4x4& world_from_clip,
    float s,
    float t
) -> RdcRay {
    // Center pixel.
    auto px = pixel_idx.x + s;
    auto py = pixel_idx.y + t;

    // Normalize.
    px /= (float)image_size.x;
    py /= (float)image_size.y;

    // Scale to [-1, 1].
    px = 2.0f * px - 1.0f;
    py = 2.0f * py - 1.0f;

    // Transform.
    const auto pxyzw = world_from_clip * float4(px, py, 0.0f, 1.0f);
    const auto pxyz = float3(pxyzw.x, pxyzw.y, pxyzw.z);
    const auto p = pxyz / pxyzw.w;

    // Ray direction.
    const auto dir = float3_normalize(p - camera_position);

    return RdcRay {
        .origin = camera_position,
        .dir = dir,
    };
}

//
// Bounding volume hierarchy.
//

struct RdcBvhPrimitive {
    uint id = {};
    float3 centroid = {};
    RdcAabb bounds = {};
};

struct RdcBvhBuildNode {
    RdcAabb bounds = rdc_aabb_new();
    std::array<uint, 2> children = {RDC_NIL, RDC_NIL};
    uint split_axis = RDC_NIL;
    uint first_primitive_offset = RDC_NIL;
    uint primitive_count = RDC_NIL;
};

auto rdc_bvh_build_node_interior(
    const RdcAabb& bounds,
    uint split_axis,
    std::array<uint, 2> children
) -> RdcBvhBuildNode {
    RdcBvhBuildNode interior_node;
    interior_node.bounds = bounds;
    interior_node.children = children;
    interior_node.split_axis = split_axis;
    return interior_node;
}

auto rdc_bvh_build_node_leaf(
    const RdcAabb& bounds,
    uint first_primitive_offset,
    uint primitive_count
) -> RdcBvhBuildNode {
    RdcBvhBuildNode leaf_node;
    leaf_node.bounds = bounds;
    leaf_node.first_primitive_offset = first_primitive_offset;
    leaf_node.primitive_count = primitive_count;
    return leaf_node;
}

auto rdc_bvh_build_recursive(
    std::vector<RdcBvhBuildNode>& build_nodes,
    std::vector<uint>& ordered_primitive_indices,
    std::span<RdcBvhPrimitive> primitives
) -> uint {
    // Validation.
    FB_ASSERT(!primitives.empty());

    // Constants.
    static constexpr uint BUCKET_COUNT = 12;
    static constexpr uint NODE_MAX_PRIMITIVE_COUNT = 255;
    static constexpr uint SAH_PRIMITIVE_THRESHOLD = 4;
    static_assert(BUCKET_COUNT > 0);
    static_assert(NODE_MAX_PRIMITIVE_COUNT > 0);
    static_assert(NODE_MAX_PRIMITIVE_COUNT < (1 << (sizeof(uint8_t) * CHAR_BIT)));
    static_assert(SAH_PRIMITIVE_THRESHOLD > 0);

    // Bucket.
    struct Bucket {
        uint count = 0;
        RdcAabb bounds = rdc_aabb_new();
    };

    // Current node.
    build_nodes.emplace_back();
    const auto curr = (uint)(build_nodes.size() - 1);
    auto curr_bounds = rdc_aabb_new();
    for (const auto& primitive : primitives) {
        curr_bounds = rdc_aabb_merged(curr_bounds, primitive.bounds);
    }

    // Only one primitive left, terminate as leaf.
    const auto primitive_count = (uint)primitives.size();
    if (primitive_count == 1) {
        build_nodes[curr] = rdc_bvh_build_node_leaf(
            curr_bounds,
            (uint)ordered_primitive_indices.size(),
            primitive_count
        );
        for (const auto& primitive : primitives) {
            ordered_primitive_indices.push_back(primitive.id);
        }
        return curr;
    }

    // Build inner node.
    auto centroid_bounds = rdc_aabb_new();
    for (const auto& primitive : primitives) {
        centroid_bounds = rdc_aabb_extended(centroid_bounds, primitive.centroid);
    }
    const float3 centroid_extents = rdc_aabb_extents(centroid_bounds);
    const uint split_axis = float3_argmax(centroid_extents);

    // Degenerate bounds, terminate as leaf.
    if (centroid_bounds.max[split_axis] == centroid_bounds.min[split_axis]) {
        build_nodes[curr] = rdc_bvh_build_node_leaf(
            curr_bounds,
            (uint)ordered_primitive_indices.size(),
            primitive_count
        );
        for (const auto& primitive : primitives) {
            ordered_primitive_indices.push_back(primitive.id);
        }
        return curr;
    }

    // Initial split point.
    auto split = primitive_count / 2;
    if (primitive_count <= SAH_PRIMITIVE_THRESHOLD) {
        // SAH computation is excessive, sort by centroid instead.
        std::sort(
            primitives.begin(),
            primitives.end(),
            [split_axis](const RdcBvhPrimitive& primitive_a, const RdcBvhPrimitive& primitive_b) {
                const auto a = primitive_a.centroid[split_axis];
                const auto b = primitive_b.centroid[split_axis];
                return a < b;
            }
        );
    } else {
        // Helper.
        const auto find_bucket_idx = [&](const RdcBvhPrimitive& primitive) -> uint {
            const auto numer = primitive.centroid[split_axis] - centroid_bounds.min[split_axis];
            const auto denom = centroid_bounds.max[split_axis] - centroid_bounds.min[split_axis];
            const auto bucket_idx =
                std::min((uint)((float)BUCKET_COUNT * numer / denom), BUCKET_COUNT - 1u);
            FB_ASSERT(bucket_idx >= 0 && bucket_idx < BUCKET_COUNT);
            return bucket_idx;
        };

        // Setup buckets.
        std::array<Bucket, BUCKET_COUNT> buckets = {};
        for (const auto& primitive : primitives) {
            auto& bucket = buckets[find_bucket_idx(primitive)];
            bucket.count += 1;
            bucket.bounds = rdc_aabb_merged(bucket.bounds, primitive.bounds);
        }

        // Bruteforce SAH cost at every split point and find smallest.
        const auto curr_bounds_area = rdc_aabb_surface_area(curr_bounds);
        auto min_cost_idx = 0xffffffffu;
        auto min_cost = FLT_MAX;
        for (uint cost_idx = 0; cost_idx < BUCKET_COUNT - 1; cost_idx++) {
            auto left_count = 0.0f;
            auto left_bounds = rdc_aabb_new();
            auto right_count = 0.0f;
            auto right_bounds = rdc_aabb_new();
            for (uint i = 0; i <= cost_idx; i++) {
                left_count += 1.0f;
                left_bounds = rdc_aabb_merged(left_bounds, buckets[i].bounds);
            }
            for (uint i = cost_idx + 1; i < BUCKET_COUNT; i++) {
                right_count += 1.0f;
                right_bounds = rdc_aabb_merged(right_bounds, buckets[i].bounds);
            }
            const auto left_surface_area = rdc_aabb_surface_area(left_bounds);
            const auto left_cost = left_count * left_surface_area;
            const auto right_surface_area = rdc_aabb_surface_area(right_bounds);
            const auto right_cost = right_count * right_surface_area;
            const auto cost = 0.125f * (left_cost + right_cost) / curr_bounds_area;
            if (cost < min_cost) {
                min_cost = cost;
                min_cost_idx = cost_idx;
            }
        }
        FB_ASSERT(min_cost_idx != 0xffffffffu);
        FB_ASSERT(min_cost != FLT_MAX);

        // Partition or terminate as leaf?
        const auto leaf_cost = (float)primitive_count;
        if (primitive_count > NODE_MAX_PRIMITIVE_COUNT || min_cost < leaf_cost) {
            // Partition around the best bucket.
            auto lhs = std::vector<RdcBvhPrimitive>();
            auto rhs = std::vector<RdcBvhPrimitive>();
            for (const auto& primitive : primitives) {
                if (find_bucket_idx(primitive) <= min_cost_idx) {
                    lhs.push_back(primitive);
                } else {
                    rhs.push_back(primitive);
                }
            }
            FB_ASSERT(lhs.size() > 0);
            FB_ASSERT(rhs.size() > 0);
            FB_ASSERT(lhs.size() + rhs.size() == primitives.size());
            split = (uint)lhs.size();
            const auto lhs_byte_count = lhs.size() * sizeof(RdcBvhPrimitive);
            const auto rhs_byte_count = rhs.size() * sizeof(RdcBvhPrimitive);
            memcpy(primitives.data(), lhs.data(), lhs_byte_count);
            memcpy(primitives.data() + lhs.size(), rhs.data(), rhs_byte_count);
        } else {
            // Splitting is too expensive, terminate as leaf.
            build_nodes[curr] = rdc_bvh_build_node_leaf(
                curr_bounds,
                (uint)ordered_primitive_indices.size(),
                primitive_count
            );
            for (const auto& primitive : primitives) {
                ordered_primitive_indices.push_back(primitive.id);
            }
            return curr;
        }
    }

    // Recurse children.
    const auto left_child = rdc_bvh_build_recursive(
        build_nodes,
        ordered_primitive_indices,
        primitives.subspan(0, split)
    );
    const auto right_child = rdc_bvh_build_recursive(
        build_nodes,
        ordered_primitive_indices,
        primitives.subspan(split, (uint)primitives.size() - split)
    );
    const auto children_bounds =
        rdc_aabb_merged(build_nodes[left_child].bounds, build_nodes[right_child].bounds);
    build_nodes[curr] =
        rdc_bvh_build_node_interior(children_bounds, split_axis, {left_child, right_child});

    return curr;
}

auto rdc_bvh_flatten_recursive(
    std::vector<RdcBvhNode>& nodes,
    uint parent,
    const std::vector<RdcBvhBuildNode>& build_nodes
) -> uint {
    // Unpack.
    const auto& parent_node = build_nodes[parent];

    // New node.
    const auto curr = (uint)nodes.size();
    nodes.emplace_back();

    // Bounds.
    nodes[curr].bounds = parent_node.bounds;

    // Leaf or interior.
    if (parent_node.primitive_count != RDC_NIL) {
        // Leaf.
        nodes[curr].offset = parent_node.first_primitive_offset;
        nodes[curr].primitive_count = (uint8_t)parent_node.primitive_count;
    } else {
        // Interior.
        nodes[curr].axis = (uint8_t)parent_node.split_axis;
        const auto left_child = parent_node.children[0];
        const auto right_child = parent_node.children[1];
        rdc_bvh_flatten_recursive(nodes, left_child, build_nodes);
        nodes[curr].offset = rdc_bvh_flatten_recursive(nodes, right_child, build_nodes);
    }

    return curr;
}

auto rdc_bvh_create(std::vector<RdcBvhNode>& nodes, std::vector<RdcTriangle>& triangles) -> void {
    // Validate.
    FB_ASSERT(nodes.empty());
    FB_ASSERT(!triangles.empty());

    // Primitives.
    std::vector<RdcBvhPrimitive> primitives;
    primitives.reserve(triangles.size());
    for (uint i = 0; i < triangles.size(); i++) {
        const auto& triangle = triangles[i];
        auto bounds = rdc_aabb_new();
        bounds = rdc_aabb_extended(bounds, triangle.positions[0]);
        bounds = rdc_aabb_extended(bounds, triangle.positions[1]);
        bounds = rdc_aabb_extended(bounds, triangle.positions[2]);
        const auto centroid = rdc_aabb_center(bounds);
        primitives.push_back({
            .id = i,
            .centroid = centroid,
            .bounds = bounds,
        });
    }
    FB_ASSERT(primitives.size() == triangles.size());

    // Build.
    std::vector<RdcBvhBuildNode> build_nodes;
    std::vector<uint> ordered_primitive_indices;
    rdc_bvh_build_recursive(build_nodes, ordered_primitive_indices, primitives);
    FB_ASSERT(ordered_primitive_indices.size() == triangles.size());

    // Flatten.
    rdc_bvh_flatten_recursive(nodes, 0, build_nodes);
    FB_ASSERT(nodes.size() == build_nodes.size());

    // Re-order.
    std::vector<RdcTriangle> ordered_triangles;
    ordered_triangles.resize(triangles.size());
    for (uint i = 0; i < ordered_primitive_indices.size(); i++) {
        ordered_triangles[i] = triangles[ordered_primitive_indices[i]];
    }
    triangles = ordered_triangles;
}

auto rdc_bvh_hit(
    const RdcRay& ray,
    std::span<const RdcBvhNode> nodes,
    std::span<const RdcTriangle> triangles,
    float& out_t,
    float3& out_uvw,
    uint& out_triangle_idx
) -> bool {
    // Intersectors.
    const auto ray_aabb = rdc_ray_aabb_intersector_create(ray);
    const auto ray_triangle = rdc_ray_triangle_intersector_create(ray);

    // Traverse.
    uint node_idx = 0u;
    uint todo_offset = 0u;
    static constexpr uint MAX_TODO = 64u;
    std::array<uint, MAX_TODO> todo = {};
    float best_t = FLT_MAX;
    bool hit = false;
    for (;;) {
        // Unpack.
        const auto& node = nodes[node_idx];
        const auto bounds = node.bounds;
        const auto offset = node.offset;
        const auto primitive_count = node.primitive_count;
        const auto axis = node.axis;

        // Test bounds.
        if (rdc_ray_aabb_intersector_hit(ray_aabb, ray, bounds, best_t)) {
            if (primitive_count > 0u) {
                // Test triangles.
                for (uint i = 0; i < primitive_count; i++) {
                    // Unpack.
                    const auto triangle_idx = offset + i;
                    const auto& triangle = triangles[triangle_idx];

                    // Test.
                    float t = best_t;
                    float3 uvw = FLOAT3_ZERO;
                    if (rdc_ray_triangle_intersector_hit(ray_triangle, ray, triangle, t, uvw)) {
                        hit = true;
                        if (t < best_t) {
                            best_t = t;
                            out_t = t;
                            out_uvw = uvw;
                            out_triangle_idx = triangle_idx;
                        }
                    }
                }

                if (todo_offset == 0u) {
                    break;
                }
                FB_ASSERT(todo_offset > 0u);
                node_idx = todo[--todo_offset];
            } else if (ray_aabb.ray_dir_neg[axis]) {
                FB_ASSERT(todo_offset < MAX_TODO);
                todo[todo_offset++] = node_idx + 1u;
                node_idx = offset;
            } else {
                FB_ASSERT(todo_offset < MAX_TODO);
                todo[todo_offset++] = offset;
                node_idx += 1u;
            }
        } else {
            if (todo_offset == 0u) {
                break;
            }
            FB_ASSERT(todo_offset > 0u);
            node_idx = todo[--todo_offset];
        }
    }

    return hit;
}

//
// Ray-triangle intersector.
//

// Implementation based on "Watertight Ray/Triangle Intersection".
// https://jcgt.org/published/0002/01/05/

auto rdc_ray_triangle_intersector_create(const RdcRay& ray) -> RdcRayTriangleIntersector {
    // Calculate dimension where the ray direction is maximal.
    uint3 k = uint3(0xffffffffu, 0xffffffffu, 0xffffffffu);
    k.z = (uint)float3_argmax(float3_abs(ray.dir));
    k.x = k.z + 1u;
    if (k.x == 3u) {
        k.x = 0u;
    }
    k.y = k.x + 1u;
    if (k.y == 3u) {
        k.y = 0u;
    }

    // Swap kx and ky dimension to preserve winding direction of triangles.
    if (ray.dir[k.z] < 0.0f) {
        std::swap(k.x, k.y);
    }

    // Calculate shear constants.
    float3 s = float3(ray.dir[k.x] / ray.dir[k.z], ray.dir[k.y] / ray.dir[k.z], 1.0 / ray.dir[k.z]);

    return RdcRayTriangleIntersector {k, s};
}

auto rdc_ray_triangle_intersector_hit(
    const RdcRayTriangleIntersector& intersector,
    const RdcRay& ray,
    const RdcTriangle& triangle,
    float& out_t,
    float3& out_uvw
) -> bool {
    // Aliases.
    const auto k = intersector.k;
    const auto s = intersector.s;

    // Unpack triangle.
    const auto a = triangle.positions[0] - ray.origin;
    const auto b = triangle.positions[1] - ray.origin;
    const auto c = triangle.positions[2] - ray.origin;

    // Perform shear and scale of vertices.
    const auto ax = a[k.x] - s.x * a[k.z];
    const auto ay = a[k.y] - s.y * a[k.z];
    const auto bx = b[k.x] - s.x * b[k.z];
    const auto by = b[k.y] - s.y * b[k.z];
    const auto cx = c[k.x] - s.x * c[k.z];
    const auto cy = c[k.y] - s.y * c[k.z];

    // Calculate scaled barycentric coordinates.
    auto u = cx * by - cy * bx;
    auto v = ax * cy - ay * cx;
    auto w = bx * ay - by * ax;

    // Fallback to test against edges using double precision.
    if (u == 0.0f && v == 0.0f && w == 0.0f) {
        const auto cxby = double(cx) * double(by);
        const auto cybx = double(cy) * double(bx);
        u = float(cxby - cybx);
        const auto axcy = double(ax) * double(cy);
        const auto aycx = double(ay) * double(cx);
        v = float(axcy - aycx);
        const auto bxay = double(bx) * double(ay);
        const auto byax = double(by) * double(ax);
        w = float(bxay - byax);
    }

    // Perform edge tests.
    if (u < 0.0f || v < 0.0f || w < 0.0f) {
        return false;
    }

    // Calculate determinant.
    const auto det = u + v + w;
    if (det == 0.0f) {
        return false;
    }

    // Calculate scaled z-coordinates of vertices and use them to calculate the hit distance.
    const auto az = s.z * a[k.z];
    const auto bz = s.z * b[k.z];
    const auto cz = s.z * c[k.z];
    const auto t = u * az + v * bz + w * cz;
    if (t < 0.0f || t > out_t * det) {
        return false;
    }

    // Normalize.
    const auto rcpdet = 1.0 / det;
    out_uvw = float3(u * rcpdet, v * rcpdet, w * rcpdet);

    // Update t-value.
    out_t = t * rcpdet;

    return true;
}

//
// Ray-aabb intersector.
//

auto rdc_ray_aabb_intersector_create(const RdcRay& ray) -> RdcRayAabbIntersector {
    RdcRayAabbIntersector intersector;
    intersector.ray_dir_inv = 1.0f / ray.dir;
    intersector.ray_dir_neg[0] = intersector.ray_dir_inv.x < 0.0f;
    intersector.ray_dir_neg[1] = intersector.ray_dir_inv.y < 0.0f;
    intersector.ray_dir_neg[2] = intersector.ray_dir_inv.z < 0.0f;
    return intersector;
}

FB_INLINE constexpr auto gamma(float n) -> float {
    const float MACHINE_EPSILON = FLT_EPSILON * 0.5f;
    return (n * MACHINE_EPSILON) / (1.0f - n * MACHINE_EPSILON);
}

auto rdc_ray_aabb_intersector_hit(
    const RdcRayAabbIntersector& intersector,
    const RdcRay& ray,
    const RdcAabb& aabb,
    float best_t
) -> bool {
    // Aliases.
    const auto ray_dir_inv = intersector.ray_dir_inv;
    const auto ray_dir_neg = intersector.ray_dir_neg;

    // Compute slab intervals.
    float mn_tx;
    float mn_ty;
    float mn_tz;
    float mx_tx;
    float mx_ty;
    float mx_tz;
    if (ray_dir_neg[0]) {
        mn_tx = aabb.max.x;
        mx_tx = aabb.min.x;
    } else {
        mn_tx = aabb.min.x;
        mx_tx = aabb.max.x;
    }
    if (ray_dir_neg[1]) {
        mn_ty = aabb.max.y;
        mx_ty = aabb.min.y;
    } else {
        mn_ty = aabb.min.y;
        mx_ty = aabb.max.y;
    }
    if (ray_dir_neg[2]) {
        mn_tz = aabb.max.z;
        mx_tz = aabb.min.z;
    } else {
        mn_tz = aabb.min.z;
        mx_tz = aabb.max.z;
    }
    mn_tx = (mn_tx - ray.origin.x) * ray_dir_inv.x;
    mn_ty = (mn_ty - ray.origin.y) * ray_dir_inv.y;
    mn_tz = (mn_tz - ray.origin.z) * ray_dir_inv.z;
    mx_tx = (mx_tx - ray.origin.x) * ray_dir_inv.x;
    mx_ty = (mx_ty - ray.origin.y) * ray_dir_inv.y;
    mx_tz = (mx_tz - ray.origin.z) * ray_dir_inv.z;

    // Ensures robust bounds intersection.
    mx_tx *= 1.0f + 2.0f * gamma(3.0f);
    mx_ty *= 1.0f + 2.0f * gamma(3.0f);
    mx_tz *= 1.0f + 2.0f * gamma(3.0f);

    // Check for intersections.
    if (mn_tx > mx_ty || mn_ty > mx_tx) {
        return false;
    }
    if (mn_ty > mn_tx) {
        mn_tx = mn_ty;
    }
    if (mx_ty < mx_tx) {
        mx_tx = mx_ty;
    }
    if (mn_tx > mx_tz || mn_tz > mx_tx) {
        return false;
    }
    if (mn_tz > mn_tx) {
        mn_tx = mn_tz;
    }
    if (mx_tz < mx_tx) {
        mx_tx = mx_tz;
    }

    return (mn_tx < best_t) && (mx_tx > 0.0f);
}

auto rdc_bruteforce_hit(
    const RdcRay& ray,
    std::span<const RdcTriangle> triangles,
    float& out_t,
    float3& out_uvw,
    uint& out_triangle_idx
) -> bool {
    const auto ray_triangle = rdc_ray_triangle_intersector_create(ray);
    auto best_t = FLT_MAX;
    auto hit = false;
    for (uint triangle_idx = 0; triangle_idx < (uint)triangles.size(); triangle_idx++) {
        const auto& triangle = triangles[triangle_idx];
        auto t = best_t;
        auto uvw = FLOAT3_ZERO;
        if (rdc_ray_triangle_intersector_hit(ray_triangle, ray, triangle, t, uvw)) {
            if (t < best_t) {
                hit = true;
                best_t = t;
                out_t = t;
                out_uvw = uvw;
                out_triangle_idx = triangle_idx;
            }
        }
    }
    return hit;
}

} // namespace fb
