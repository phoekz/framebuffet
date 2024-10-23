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
    //    [x] Texture sampling
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
        // const auto& mesh = assets.rounded_cube_mesh();
        const auto& mesh = assets.plane_mesh();
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

        auto texture = assets.plane_texture();

        const auto sun_azimuth = 0.0f;                  // [0, 360].
        const auto sun_elevation = rad_from_deg(30.0f); // [0, 90].
        RdcSkyState sky;
        RdcSkyParams sky_params;
        sky_params.elevation = sun_elevation;
        sky_params.turbidity = 1;
        sky_params.albedo[0] = 1.0f;
        sky_params.albedo[1] = 1.0f;
        sky_params.albedo[2] = 1.0f;
        RdcSkyStateResult sky_result = rdc_sky_create(sky, sky_params);
        FB_ASSERT(sky_result == RdcSkyStateResult::Success);
        const auto sun_angle_xz = sun_azimuth;
        const auto sun_angle_y = 0.5f * FLOAT_PI - sun_elevation;
        const auto sun_dir = float3(
            std::sin(sun_angle_y) * std::cos(sun_angle_xz),
            std::cos(sun_angle_y),
            std::sin(sun_angle_y) * std::sin(sun_angle_xz)
        );

        const auto exposure = exposure_from_stops(4.0f);

        const auto sample_count = 32u;
        const auto bounce_count = 4u;
        const auto image_scale = 25u;
        const auto image_size = uint2(16u * image_scale, 10u * image_scale);
        const auto image_aspect = (float)image_size.x / (float)image_size.y;
        const auto camera_fovy = rad_from_deg(60.0f);
        const auto camera_position = float3(1.5f, 1.0f, 1.5f);
        const auto view_from_world =
            float4x4_lookat(camera_position, float3(0.0f), float3(0.0f, 1.0f, 0.0f));
        const auto clip_from_view = float4x4_perspective(camera_fovy, image_aspect, 0.1f, 100.0f);
        const auto clip_from_world = clip_from_view * view_from_world;
        const auto world_from_clip = float4x4_inverse(clip_from_world);
        auto image = std::vector<RgbaByte>(image_size.x * image_size.y);

        auto rng = Pcg();
        auto hit_count = 0u;
        auto ray_count = 0u;
        for (uint pixel_y = 0u; pixel_y < image_size.y; pixel_y++) {
            for (uint pixel_x = 0u; pixel_x < image_size.x; pixel_x++) {
                float3 sum_radiance = FLOAT3_ZERO;
                for (uint sample_idx = 0u; sample_idx < sample_count; sample_idx++) {
                    // Primary ray.
                    const RdcRay primary_ray = rdc_ray_from_sensor(
                        image_size,
                        {pixel_x, pixel_y},
                        camera_position,
                        world_from_clip,
                        rng.random_float(),
                        rng.random_float()
                    );

                    RdcRay ray = primary_ray;
                    float3 radiance = FLOAT3_ZERO;
                    float3 throughput = FLOAT3_ONE;
                    for (uint bounce_idx = 0u; bounce_idx < bounce_count; bounce_idx++) {
                        // BVH test.
                        float out_t;
                        float3 out_uvw;
                        uint out_triangle_idx;
                        ray_count++;
                        if (rdc_bvh_hit(ray, nodes, triangles, out_t, out_uvw, out_triangle_idx)) {
                            // Unpack attributes.
                            const auto& triangle = triangles[out_triangle_idx];
                            const auto texcoord =                   //
                                out_uvw.x * triangle.texcoords[0] + //
                                out_uvw.y * triangle.texcoords[1] + //
                                out_uvw.z * triangle.texcoords[2];
                            const auto normal = float3_normalize(
                                out_uvw.x * triangle.normals[0] + //
                                out_uvw.y * triangle.normals[1] + //
                                out_uvw.z * triangle.normals[2]
                            );

                            // Sample texture.
                            const auto texel = rdc_texture_sample(texture, texcoord);
                            const auto texel_rgb = float3(texel.x, texel.y, texel.z);

                            // Sample next direction.
                            const auto onb = rdc_orthonormal_basis_from_normal(normal);
                            const auto sample = rdc_hemisphere_uniform_sample(
                                texel_rgb,
                                rng.random_float(),
                                rng.random_float()
                            );
                            const auto wi_local = sample.wi;
                            const auto wi_world = float3_normalize(onb.world_from_local * wi_local);
                            const auto wo_world = -ray.dir;
                            ray.origin += 0.999f * out_t * ray.dir;
                            ray.dir = wi_world;

                            // Shading.
                            const auto cos_theta = std::abs(float3_dot(wo_world, normal));
                            throughput *= sample.r * cos_theta / sample.pdf;

                            hit_count++;
                        } else {
                            // Skylight model.
                            const auto theta = std::acos(ray.dir.y);
                            const auto cos_gamma =
                                std::clamp(float3_dot(ray.dir, sun_dir), -1.0f, 1.0f);
                            const auto gamma = std::acos(cos_gamma);
                            const auto sky_radiance = float3(
                                rdc_sky_radiance(sky, theta, gamma, RdcSkyChannel::R),
                                rdc_sky_radiance(sky, theta, gamma, RdcSkyChannel::G),
                                rdc_sky_radiance(sky, theta, gamma, RdcSkyChannel::B)
                            );
                            radiance += throughput * sky_radiance;
                            break;
                        }
                    }
                    FB_ASSERT(float_isfinite(radiance.x));
                    FB_ASSERT(float_isfinite(radiance.y));
                    FB_ASSERT(float_isfinite(radiance.z));
                    sum_radiance += radiance;
                }
                sum_radiance /= (float)sample_count;

                // Update pixel.
                {
                    const auto linear_color = float3(
                        tonemap_aces(exposure * sum_radiance.x),
                        tonemap_aces(exposure * sum_radiance.y),
                        tonemap_aces(exposure * sum_radiance.z)
                    );
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
                }
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
