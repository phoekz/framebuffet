#include "rdc.hpp"

#include <stb_image_write.h>

namespace fb {

auto rdc_render(const baked::raydiance::Assets& assets, const baked::raydiance::Shaders& shaders)
    -> void {
    // Todo:
    // [x] Sensor size, camera parameters, inverse projection, world_from_clip, camera_position.
    // [x] Scene geometry loading.
    //    [x] BVH building
    //       [x] AABB functions
    //       [x] rdc_bvh_build_recursive
    //       [x] rdc_bvh_flatten_recursive
    //       [x] Re-order triangles
    //    [x] Ray-triangle intersection.
    //    [x] Ray-box intersection.
    //    [x] BVH traversal.
    // [ ] Materials, BxDFs, sampling, orthonormal basis, skylight model.
    //    [x] Camera rays
    // [ ] Textures, mipmaps, bilinear filtering.
    // [ ] Improved scene loading, maybe USD?
    //    [ ] Material index
    //    [ ] Texture index
    //    [ ] Instancing
    // [ ] Postprocessing, exposure, write to disk, upload to gpu, display to screen.
    //    [x] sRGB color correction.
    // [ ] Performance, multithreading, restart-on-change.
    // [ ] Unit testing with catch2.

    // Test raytracing.
    {
        std::vector<RdcTriangle> triangles;
        // const auto& mesh = assets.cube_mesh();
        // const auto& mesh = assets.sphere_mesh();
        const auto& mesh = assets.rounded_cube_mesh();
        for (const auto& submesh : mesh.submeshes) {
            const auto vertices = mesh.vertices;
            const auto indices = mesh.indices;
            const auto transform = FLOAT4X4_IDENTITY;
            for (uint i = submesh.start_index; i < submesh.index_count; i += 3) {
                const auto i0 = indices[i + 0];
                const auto i1 = indices[i + 1];
                const auto i2 = indices[i + 2];
                const auto& v0 = vertices[i0];
                const auto& v1 = vertices[i1];
                const auto& v2 = vertices[i2];
                RdcTriangle triangle;
                triangle.positions[0] = float4x4_transform_point(transform, v0.position);
                triangle.positions[1] = float4x4_transform_point(transform, v1.position);
                triangle.positions[2] = float4x4_transform_point(transform, v2.position);
                triangle.texcoords[0] = v0.texcoord;
                triangle.texcoords[1] = v1.texcoord;
                triangle.texcoords[2] = v2.texcoord;
                triangle.normals[0] = v0.normal;
                triangle.normals[1] = v1.normal;
                triangle.normals[2] = v2.normal;
                triangles.push_back(triangle);
            }
        }

        std::vector<RdcBvhNode> nodes;
        rdc_bvh_create(nodes, triangles);

        const auto image_scale = 25;
        const auto image_size = uint2(16 * image_scale, 10 * image_scale);
        const auto image_aspect = (float)image_size.x / (float)image_size.y;
        const auto camera_fovy = rad_from_deg(60.0f);
        const auto camera_position = 2.0f * FLOAT3_ONE;
        const auto view_from_world =
            float4x4_lookat(camera_position, float3(0.0f), float3(0.0f, 1.0f, 0.0f));
        const auto clip_from_view = float4x4_perspective(camera_fovy, image_aspect, 0.1f, 100.0f);
        const auto clip_from_world = clip_from_view * view_from_world;
        const auto world_from_clip = float4x4_inverse(clip_from_world);
        auto image = std::vector<RgbaByte>(image_size.x * image_size.y);
        auto hit_count = 0u;
        auto ray_count = 0u;
        for (uint pixel_y = 0; pixel_y < image_size.y; pixel_y++) {
            for (uint pixel_x = 0; pixel_x < image_size.x; pixel_x++) {
                const RdcRay ray = rdc_ray_from_sensor(
                    image_size,
                    {pixel_x, pixel_y},
                    camera_position,
                    world_from_clip,
                    0.5f,
                    0.5f
                );

                float out_t;
                float3 out_uvw;
                uint out_triangle_idx;
                if (rdc_bvh_hit(ray, nodes, triangles, out_t, out_uvw, out_triangle_idx)) {
                    // Hit.
                    const auto& triangle = triangles[out_triangle_idx];
                    // const auto texcoord = out_uvw.x * triangle.texcoords[0] + //
                    //     out_uvw.y * triangle.texcoords[1] +                   //
                    //     out_uvw.z * triangle.texcoords[2];
                    const auto normal = float3_normalize(
                        out_uvw.x * triangle.normals[0] + //
                        out_uvw.y * triangle.normals[1] + //
                        out_uvw.z * triangle.normals[2]
                    );
                    const auto linear_color = float3(0.5f) + 0.5f * normal;
                    // const auto linear_color = float3(texcoord.x, texcoord.y, 0.0f);
                    const auto srgb_color = float3(
                        srgb_from_linear(linear_color.x),
                        srgb_from_linear(linear_color.y),
                        srgb_from_linear(linear_color.z)
                    );
                    const RgbaByte color = {
                        (uint8_t)(255.0f * srgb_color.x),
                        (uint8_t)(255.0f * srgb_color.y),
                        (uint8_t)(255.0f * srgb_color.z),
                        255,
                    };

                    const uint px = pixel_x;
                    const uint py = image_size.y - pixel_y - 1;
                    const uint pixel_idx = py * image_size.x + px;
                    image[pixel_idx] = color;

                    hit_count++;
                } else {
                    // Miss.
                    const uint px = pixel_x;
                    const uint py = image_size.y - pixel_y - 1;
                    const uint pixel_idx = py * image_size.x + px;
                    image[pixel_idx] = RgbaByte(32, 32, 32, 255);
                }

                ray_count++;
            }
        }
        stbi_write_png("rdc_render.png", image_size.x, image_size.y, 4, image.data(), 0);
        FB_LOG_INFO("hit_count={} ray_count={}", hit_count, ray_count);

        if (1) {
            exit(0);
        }
    }

    // Test ray-triangle intersection.
    {
        RdcRay ray = {
            .origin = float3(0.0f, 0.0f, -1.0f),
            .dir = float3(0.0f, 0.0f, 1.0f),
        };
        RdcTriangle triangle = {
            .positions =
                {
                    float3(-1.0f, -1.0f, 0.0f),
                    float3(0.0f, 1.0f, 0.0f),
                    float3(1.0f, -1.0f, 0.0f),
                },
        };
        RdcRayTriangleIntersector ray_triangle;
        ray_triangle = rdc_ray_triangle_intersector_create(ray);
        float t = FLT_MAX;
        float3 uvw = FLOAT3_ZERO;
        const bool hit = rdc_ray_triangle_intersector_hit(ray_triangle, ray, triangle, t, uvw);
        FB_LOG_INFO("{} {} {}", t, uvw, hit);
    }

    // Test ray-aabb intersection.
    {
        RdcRay ray = {
            .origin = float3(0.0f, 0.0f, -1.0f),
            .dir = float3(0.0f, 0.0f, 1.0f),
        };
        RdcAabb aabb = {
            .min = float3(-0.5f),
            .max = float3(0.5f),
        };
        RdcRayAabbIntersector ray_aabb;
        ray_aabb = rdc_ray_aabb_intersector_create(ray);
        float t = FLT_MAX;
        const bool hit = rdc_ray_aabb_intersector_hit(ray_aabb, ray, aabb, t);
        FB_LOG_INFO("{}", hit);
    }
}

} // namespace fb
