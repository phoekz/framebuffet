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
        const auto& cube_mesh = assets.rounded_cube_mesh();
        const auto& plane_mesh = assets.plane_mesh();
        enum class MaterialId : uint {
            Cube,
            Plane,
        };
        struct MeshMaterialId {
            const baked::Mesh& mesh;
            MaterialId material;
        };
        for (const auto& [mesh, material] : {
                 MeshMaterialId {cube_mesh, MaterialId::Cube},
                 MeshMaterialId {plane_mesh, MaterialId::Plane},
             }) {
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
                    triangle.material = (uint)material;
                    triangles.push_back(triangle);
                }
            }
        }

        std::vector<RdcBvhNode> nodes;
        rdc_bvh_create(nodes, triangles);

        const auto textures =
            std::to_array({assets.rounded_cube_base_color_texture(), assets.plane_texture()});

        const auto sun_azimuth = 0.0f;                  // [0, 360].
        const auto sun_elevation = rad_from_deg(45.0f); // [0, 90].
        RdcSkyState sky;
        RdcSkyParams sky_params;
        sky_params.elevation = sun_elevation;
        sky_params.turbidity = 3;
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

        const auto exposure = exposure_from_stops(5.0f);

        const auto sample_count = 256u;
        const auto bounce_count = 16u;
        const auto image_scale = 25u;
        const auto image_size = uint2(16u * image_scale, 10u * image_scale);
        const auto image_aspect = (float)image_size.x / (float)image_size.y;
        const auto camera_fovy = rad_from_deg(60.0f);
        const auto camera_angle_x = rad_from_deg(-30.0f);
        const auto camera_distance = 2.0f;
        const auto camera_position = float3(
            camera_distance * std::cos(camera_angle_x),
            1.5f,
            camera_distance * std::sin(camera_angle_x)
        );
        const auto view_from_world =
            float4x4_lookat(camera_position, float3(0.0f), float3(0.0f, 1.0f, 0.0f));
        const auto clip_from_view = float4x4_perspective(camera_fovy, image_aspect, 0.1f, 100.0f);
        const auto clip_from_world = clip_from_view * view_from_world;
        const auto world_from_clip = float4x4_inverse(clip_from_world);

        auto rendered_image = std::vector<RgbaByte>(image_size.x * image_size.y, COLOR_BLACK);
        auto texcoord_image = std::vector<RgbaByte>(image_size.x * image_size.y, COLOR_BLACK);
        auto normal_image = std::vector<RgbaByte>(image_size.x * image_size.y, COLOR_BLACK);
        auto bounce_image = std::vector<RgbaByte>(image_size.x * image_size.y, COLOR_BLACK);

        constexpr uint MAX_BOUNCE_COUNT = 16;
        struct Stats {
            uint hit_count = 0u;
            uint ray_count = 0u;
            std::array<uint, MAX_BOUNCE_COUNT> bounce_histogram = {};
        };
        auto stats = Stats {};

        auto rng = Pcg();
        for (uint pixel_y = 0u; pixel_y < image_size.y; pixel_y++) {
            for (uint pixel_x = 0u; pixel_x < image_size.x; pixel_x++) {
                // Estimate radiance.
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
                    uint bounce_idx = 0u;
                    for (; bounce_idx < bounce_count; bounce_idx++) {
                        // BVH test.
                        float out_t;
                        float3 out_uvw;
                        uint out_triangle_idx;
                        stats.ray_count++;
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
                            const auto texel =
                                rdc_texture_sample(textures[triangle.material], texcoord);
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
                            const auto cos_theta = std::abs(float3_dot(wi_world, normal));
                            throughput *= sample.r * cos_theta / sample.pdf;

                            // Debug.
                            stats.hit_count++;
                            if (sample_idx == 0u && bounce_idx == 0u) {
                                const uint px = pixel_x;
                                const uint py = image_size.y - pixel_y - 1;
                                const uint pixel_idx = px + py * image_size.x;
                                texcoord_image[pixel_idx] = RgbaByte {
                                    (uint8_t)(255.0f * srgb_from_linear(texcoord.x)),
                                    (uint8_t)(255.0f * srgb_from_linear(texcoord.y)),
                                    0,
                                    255,
                                };
                                normal_image[pixel_idx] = RgbaByte {
                                    (uint8_t)(255.0f * srgb_from_linear(0.5f * normal.x + 0.5f)),
                                    (uint8_t)(255.0f * srgb_from_linear(0.5f * normal.y + 0.5f)),
                                    (uint8_t)(255.0f * srgb_from_linear(0.5f * normal.z + 0.5f)),
                                    255,
                                };
                            }
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

                    // Debug.
                    FB_ASSERT(bounce_idx <= MAX_BOUNCE_COUNT);
                    stats.bounce_histogram[bounce_idx - 1]++;
                    if (sample_idx == 0u) {
                        const uint px = pixel_x;
                        const uint py = image_size.y - pixel_y - 1;
                        const uint pixel_idx = px + py * image_size.x;
                        const float normalized_bounce_idx = (float)bounce_idx / (float)bounce_count;
                        bounce_image[pixel_idx] = RgbaByte {
                            (uint8_t)(255.0f * srgb_from_linear(normalized_bounce_idx)),
                            (uint8_t)(255.0f * srgb_from_linear(normalized_bounce_idx)),
                            (uint8_t)(255.0f * srgb_from_linear(normalized_bounce_idx)),
                            255,
                        };
                    }
                }
                sum_radiance /= (float)sample_count;

                // Update pixel.
                {
                    const auto linear_color = float3(
                        tonemap_aces(exposure * sum_radiance.x),
                        tonemap_aces(exposure * sum_radiance.y),
                        tonemap_aces(exposure * sum_radiance.z)
                    );
                    FB_ASSERT(linear_color.x >= 0.0f && linear_color.x <= 1.0f);
                    FB_ASSERT(linear_color.y >= 0.0f && linear_color.y <= 1.0f);
                    FB_ASSERT(linear_color.z >= 0.0f && linear_color.z <= 1.0f);
                    const auto srgb_color = float3(
                        srgb_from_linear(linear_color.x),
                        srgb_from_linear(linear_color.y),
                        srgb_from_linear(linear_color.z)
                    );
                    FB_ASSERT(srgb_color.x >= 0.0f && srgb_color.x <= 1.0f);
                    FB_ASSERT(srgb_color.y >= 0.0f && srgb_color.y <= 1.0f);
                    FB_ASSERT(srgb_color.z >= 0.0f && srgb_color.z <= 1.0f);
                    const RgbaByte final_color = {
                        (uint8_t)(255.0f * srgb_color.x),
                        (uint8_t)(255.0f * srgb_color.y),
                        (uint8_t)(255.0f * srgb_color.z),
                        255,
                    };
                    const uint px = pixel_x;
                    const uint py = image_size.y - pixel_y - 1;
                    const uint pixel_idx = px + py * image_size.x;
                    rendered_image[pixel_idx] = final_color;
                }
            }
        }

        const auto rendered_image_path = make_absolute_path("rdc_render.png");
        const auto texcoord_image_path = make_absolute_path("rdc_texcoord.png");
        const auto normal_image_path = make_absolute_path("rdc_normal.png");
        const auto bounce_image_path = make_absolute_path("rdc_bounce.png");
        stbi_write_png(
            rendered_image_path.c_str(),
            image_size.x,
            image_size.y,
            4,
            rendered_image.data(),
            0
        );
        stbi_write_png(
            texcoord_image_path.c_str(),
            image_size.x,
            image_size.y,
            4,
            texcoord_image.data(),
            0
        );
        stbi_write_png(
            normal_image_path.c_str(),
            image_size.x,
            image_size.y,
            4,
            normal_image.data(),
            0
        );
        stbi_write_png(
            bounce_image_path.c_str(),
            image_size.x,
            image_size.y,
            4,
            bounce_image.data(),
            0
        );
        FB_LOG_INFO(
            "path={}, hit_count={}, ray_count={}",
            rendered_image_path,
            stats.hit_count,
            stats.ray_count
        );
        for (uint i = 0; i < 16; i++) {
            FB_LOG_INFO(
                "bounce_histogram[{}]={} ({:.3f}%)",
                i,
                stats.bounce_histogram[i],
                100.0f * (float)stats.bounce_histogram[i] / (float)stats.ray_count
            );
        }

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
