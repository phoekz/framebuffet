#include "tasks.hpp"
#include "../formats/gltf.hpp"
#include "../formats/mikktspace.hpp"
#include "../utils/names.hpp"

#include <ttf2mesh.h>
#include <stb_image_resize2.h>

namespace fb {

class AssetsWriter {
public:
    AssetsWriter(std::vector<std::byte>& data)
        : _data(data) {}

    template<typename T>
    auto write(std::string_view type, std::span<const T> elements) -> AssetSpan {
        auto offset = _data.size();
        _data.resize(offset + elements.size_bytes());
        std::memcpy(_data.data() + offset, elements.data(), elements.size_bytes());
        return {
            .type = std::string(type),
            .offset = offset,
            .element_count = elements.size(),
            .byte_count = elements.size_bytes(),
        };
    }

private:
    std::vector<std::byte>& _data;
};

auto mipmapped_texture_asset(
    AssetsWriter& assets_writer,
    const std::string& texture_name,
    const LdrImage& texture,
    DXGI_FORMAT texture_format,
    AssetColorSpace color_space
) -> Asset {
    // Mipmapper state.
    std::array<AssetTextureData, MAX_MIP_COUNT> texture_datas = {};
    uint texture_data_count = 0;
    const uint mip_count = mip_count_from_size(texture.size());
    FB_ASSERT(mip_count <= MAX_MIP_COUNT);
    uint src_width = 0;
    uint src_height = 0;
    std::vector<std::byte> src_buffer;
    uint dst_width = 0;
    uint dst_height = 0;
    std::vector<std::byte> dst_buffer;

    // First mip can simply be copied.
    dst_width = texture.width();
    dst_height = texture.height();
    auto texture_data = texture.data();
    dst_buffer.assign(texture_data.begin(), texture_data.end());
    texture_datas[texture_data_count++] = AssetTextureData {
        .row_pitch = dst_width * texture.channel_count(),
        .slice_pitch = dst_width * dst_height * texture.channel_count(),
        .data = assets_writer.write("std::byte", std::span<const std::byte>(dst_buffer)),
    };

    // Compute the rest of the mip levels.
    for (uint mip = 1; mip < mip_count; mip++) {
        // Start by copying the previous mip level into the source buffer.
        src_width = dst_width;
        src_height = dst_height;
        src_buffer = dst_buffer;

        // Calculate the size of the next mip level.
        dst_width = std::max(1u, src_width / 2);
        dst_height = std::max(1u, src_height / 2);

        // Resize the destination buffer to fit the next mip level.
        dst_buffer.resize(dst_width * dst_height * texture.channel_count());

        // Resize the image.
        if (color_space == AssetColorSpace::Srgb) {
            stbir_resize_uint8_srgb(
                (const uint8_t*)src_buffer.data(),
                (int)src_width,
                (int)src_height,
                (int)(src_width * texture.channel_count()),
                (uint8_t*)dst_buffer.data(),
                (int)dst_width,
                (int)dst_height,
                (int)(dst_width * texture.channel_count()),
                (stbir_pixel_layout)texture.channel_count()
            );
        } else {
            stbir_resize_uint8_linear(
                (const uint8_t*)src_buffer.data(),
                (int)src_width,
                (int)src_height,
                (int)(src_width * texture.channel_count()),
                (uint8_t*)dst_buffer.data(),
                (int)dst_width,
                (int)dst_height,
                (int)(dst_width * texture.channel_count()),
                (stbir_pixel_layout)texture.channel_count()
            );
        }

        // Write mip.
        texture_datas[texture_data_count++] = AssetTextureData {
            .row_pitch = dst_width * texture.channel_count(),
            .slice_pitch = dst_width * dst_height * texture.channel_count(),
            .data = assets_writer.write("std::byte", std::span<const std::byte>(dst_buffer)),
        };
    }

    // Return.
    return AssetTexture {
        .name = texture_name,
        .format = texture_format,
        .width = texture.width(),
        .height = texture.height(),
        .mip_count = mip_count,
        .datas = texture_datas,
    };
}

auto create_box(
    std::vector<float3>& positions,
    std::vector<float3>& normals,
    std::vector<float2>& texcoords,
    std::vector<uint>& indices,
    float3 extents,
    bool rhcoords,
    bool invertn
) -> void {
    // Adapted from DirectXTK12.

    static constexpr uint FACE_COUNT = 6;
    static constexpr float3 FACE_NORMALS[FACE_COUNT] = {
        {0.0f, 0.0f, 1.0f},
        {0.0f, 0.0f, -1.0f},
        {1.0f, 0.0f, 0.0f},
        {-1.0f, 0.0f, 0.0f},
        {0.0f, 1.0f, 0.0f},
        {0.0f, -1.0f, 0.0f},
    };
    static constexpr float2 TEXCOORDS[4] = {
        {1.0f, 0.0f},
        {1.0f, 1.0f},
        {0.0f, 1.0f},
        {0.0f, 0.0f},
    };
    const float3 half_extents = extents / 2.0f;
    for (uint i = 0; i < FACE_COUNT; i++) {
        const float3 normal = FACE_NORMALS[i];
        const float3 basis = i >= 4 ? float3(0.0f, 0.0f, 1.0f) : float3(0.0f, 1.0f, 0.0f);
        const float3 side1 = float3_cross(normal, basis);
        const float3 side2 = float3_cross(normal, side1);
        const uint base_vertex = (uint)positions.size();
        indices.push_back(base_vertex + 0);
        indices.push_back(base_vertex + 1);
        indices.push_back(base_vertex + 2);
        indices.push_back(base_vertex + 0);
        indices.push_back(base_vertex + 2);
        indices.push_back(base_vertex + 3);
        positions.push_back(half_extents * (normal - side1 - side2));
        positions.push_back(half_extents * (normal - side1 + side2));
        positions.push_back(half_extents * (normal + side1 + side2));
        positions.push_back(half_extents * (normal + side1 - side2));
        normals.push_back(normal);
        normals.push_back(normal);
        normals.push_back(normal);
        normals.push_back(normal);
        texcoords.push_back(TEXCOORDS[0]);
        texcoords.push_back(TEXCOORDS[1]);
        texcoords.push_back(TEXCOORDS[2]);
        texcoords.push_back(TEXCOORDS[3]);
    }

    if (!rhcoords) {
        FB_ASSERT((indices.size() % 3) == 0);
        for (uint i = 0; i < indices.size(); i += 3) {
            std::swap(indices[i + 0], indices[i + 2]);
        }
        for (uint i = 0; i < texcoords.size(); i++) {
            texcoords[i].x = (1.0f - texcoords[i].x);
        }
    }

    if (invertn) {
        for (uint i = 0; i < normals.size(); i++) {
            normals[i] = -normals[i];
        }
    }

    FB_ASSERT(positions.size() == normals.size());
    FB_ASSERT(positions.size() == texcoords.size());
}

auto create_sphere(
    std::vector<float3>& positions,
    std::vector<float3>& normals,
    std::vector<float2>& texcoords,
    std::vector<uint>& indices,
    float diameter,
    uint tessellation,
    bool rhcoords,
    bool invertn
) -> void {
    // Adapted from DirectXTK12.

    const uint vertical_segments = tessellation;
    const uint horizontal_segments = tessellation * 2;
    const float radius = diameter / 2.0f;
    for (uint i = 0; i <= vertical_segments; i++) {
        const float v = 1.0f - float(i) / float(vertical_segments);
        const float latitude = (float(i) * FLOAT_PI / float(vertical_segments)) - 0.5f * FLOAT_PI;
        const float dy = std::sin(latitude);
        const float dxz = std::cos(latitude);
        for (uint j = 0; j <= horizontal_segments; j++) {
            const float u = float(j) / float(horizontal_segments);
            const float longitude = float(j) * 2.0f * FLOAT_PI / float(horizontal_segments);
            const float dx = std::sin(longitude);
            const float dz = std::cos(longitude);
            const float3 normal = float3(dx * dxz, dy, dz * dxz);
            const float2 texcoord = float2(u, v);
            positions.push_back(normal * radius);
            normals.push_back(normal);
            texcoords.push_back(texcoord);
        }
    }

    const uint stride = horizontal_segments + 1;
    for (uint i = 0; i < vertical_segments; i++) {
        for (uint j = 0; j <= horizontal_segments; j++) {
            const uint next_i = i + 1;
            const uint next_j = (j + 1) % stride;
            indices.push_back(i * stride + j);
            indices.push_back(next_i * stride + j);
            indices.push_back(i * stride + next_j);
            indices.push_back(i * stride + next_j);
            indices.push_back(next_i * stride + j);
            indices.push_back(next_i * stride + next_j);
        }
    }

    if (!rhcoords) {
        for (uint i = 0; i < indices.size(); i += 3) {
            std::swap(indices[i + 0], indices[i + 2]);
        }
        for (uint i = 0; i < texcoords.size(); i++) {
            texcoords[i].x = 1.0f - texcoords[i].x;
        }
    }

    if (invertn) {
        for (uint i = 0; i < normals.size(); i++) {
            normals[i] = -normals[i];
        }
    }

    FB_ASSERT(positions.size() == normals.size());
    FB_ASSERT(positions.size() == texcoords.size());
}

auto bake_assets(std::string_view assets_dir, std::span<const AssetTask> asset_tasks)
    -> std::tuple<std::vector<Asset>, std::vector<std::byte>> {
    auto assets = std::vector<Asset>();
    auto assets_bin = std::vector<std::byte>();
    auto assets_writer = AssetsWriter(assets_bin);
    auto names = UniqueNames();
    FB_LOG_INFO("Baking {} asset tasks", asset_tasks.size());
    size_t asset_index = 0;
    for (const auto& asset_task : asset_tasks) {
        // Log.
        FB_LOG_INFO(
            "{}/{} - {}",
            ++asset_index,
            asset_tasks.size(),
            asset_task_name(asset_task.index())
        );

        // Match.
        std::visit(
            overloaded {
                [&](const AssetTaskCopy& task) {
                    const auto path = std::format("{}/{}", assets_dir, task.path);
                    const auto file = read_whole_file(path);
                    assets.emplace_back(AssetCopy {
                        .name = names.unique(std::string(task.name)),
                        .data = assets_writer.write("std::byte", std::span(file)),
                    });
                },
                [&](const AssetTaskTexture& task) {
                    const auto path = std::format("{}/{}", assets_dir, task.path);
                    const auto file = read_whole_file(path);
                    const auto image = LdrImage::from_image(file);
                    assets.push_back(mipmapped_texture_asset(
                        assets_writer,
                        names.unique(std::format("{}_texture", task.name)),
                        image,
                        task.format,
                        task.color_space
                    ));
                },
                [&](const AssetTaskHdrTexture& task) {
                    const auto file = read_whole_file(std::format("{}/{}", assets_dir, task.path));
                    const auto image = HdrImage::from_image(file);
                    assets.emplace_back(AssetTexture {
                        .name = names.unique(std::format("{}_hdr_texture", task.name)),
                        .format = image.format(),
                        .width = image.width(),
                        .height = image.height(),
                        .mip_count = 1,
                        .datas = {AssetTextureData {
                            .row_pitch = image.row_pitch(),
                            .slice_pitch = image.slice_pitch(),
                            .data = assets_writer.write("std::byte", image.data()),
                        }},
                    });
                },
                [&](const AssetTaskGltf& task) {
                    // Load GLTF.
                    const auto path = std::format("{}/{}", assets_dir, task.path);
                    GltfModel model(path);
                    const auto positions = model.vertex_positions();
                    const auto normals = model.vertex_normals();
                    const auto texcoords = model.vertex_texcoords();
                    const auto joints = model.vertex_joints();
                    const auto weights = model.vertex_weights();
                    const auto indices = model.indices();
                    const auto submeshes = model.submeshes();
                    auto tangents = std::vector<float4>(positions.size());
                    generate_tangents(GenerateTangentsDesc {
                        .positions = positions,
                        .normals = normals,
                        .texcoords = texcoords,
                        .indices = indices,
                        .tangents = std::span(tangents),
                    });

                    // Submeshes.
                    auto asset_submeshes = std::vector<AssetSubmesh>();
                    for (const auto& submesh : submeshes) {
                        asset_submeshes.push_back(AssetSubmesh {
                            .index_count = submesh.index_count,
                            .start_index = submesh.start_index,
                            .base_vertex = 0,
                        });
                    }

                    // Animated vs non-animated.
                    if (joints.empty()) {
                        auto vertices = std::vector<AssetVertex>(positions.size());
                        for (size_t i = 0; i < vertices.size(); ++i) {
                            vertices[i] = AssetVertex {
                                .position = positions[i],
                                .normal = normals[i],
                                .texcoord = texcoords[i],
                                .tangent = tangents[i],
                            };
                        }

                        assets.emplace_back(AssetMesh {
                            .name = names.unique(std::format("{}_mesh", task.name)),
                            .transform = model.root_transform(),
                            .vertices = assets_writer.write(
                                "Vertex",
                                std::span<const AssetVertex>(vertices)
                            ),
                            .indices = assets_writer.write("Index", indices),
                            .submeshes = assets_writer.write(
                                "Submesh",
                                std::span<const AssetSubmesh>(asset_submeshes)
                            ),
                        });
                    } else {
                        auto vertices = std::vector<AssetSkinningVertex>(positions.size());
                        for (size_t i = 0; i < vertices.size(); ++i) {
                            vertices[i] = AssetSkinningVertex {
                                .position = positions[i],
                                .normal = normals[i],
                                .texcoord = texcoords[i],
                                .tangent = tangents[i],
                                .joint = joints[i],
                                .weight = weights[i],
                            };
                        }

                        assets.emplace_back(AssetAnimationMesh {
                            .name = names.unique(std::format("{}_animation_mesh", task.name)),
                            .transform = model.root_transform(),
                            .node_count = model.node_count(),
                            .joint_count = model.joint_count(),
                            .duration = model.animation_duration(),
                            .skinning_vertices = assets_writer.write(
                                "SkinningVertex",
                                std::span<const AssetSkinningVertex>(vertices)
                            ),
                            .indices = assets_writer.write("Index", indices),
                            .submeshes = assets_writer.write(
                                "Submesh",
                                std::span<const AssetSubmesh>(asset_submeshes)
                            ),
                            .joint_nodes = assets_writer.write("uint", model.joint_nodes()),
                            .joint_inverse_binds =
                                assets_writer.write("float4x4", model.joint_inverse_binds()),
                            .node_parents = assets_writer.write("uint", model.node_parents()),
                            .node_channels =
                                assets_writer.write("AnimationChannel", model.node_channels()),
                            .node_channels_times_t =
                                assets_writer.write("float", model.node_channels_times_t()),
                            .node_channels_times_r =
                                assets_writer.write("float", model.node_channels_times_r()),
                            .node_channels_times_s =
                                assets_writer.write("float", model.node_channels_times_s()),
                            .node_channels_values_t =
                                assets_writer.write("float3", model.node_channels_values_t()),
                            .node_channels_values_r =
                                assets_writer.write("float_quat", model.node_channels_values_r()),
                            .node_channels_values_s =
                                assets_writer.write("float3", model.node_channels_values_s()),
                        });
                    }

                    // Textures.
                    {
                        assets.push_back(mipmapped_texture_asset(
                            assets_writer,
                            names.unique(std::format("{}_base_color_texture", task.name)),
                            model.base_color_texture(),
                            GLTF_BASE_COLOR_TEXTURE_FORMAT,
                            AssetColorSpace::Srgb
                        ));
                    }
                    if (model.normal_texture().has_value()) {
                        assets.push_back(mipmapped_texture_asset(
                            assets_writer,
                            names.unique(std::format("{}_normal_texture", task.name)),
                            model.normal_texture().value().get(),
                            GLTF_NORMAL_TEXTURE_FORMAT,
                            AssetColorSpace::Linear
                        ));
                    }
                    if (model.metallic_roughness_texture().has_value()) {
                        assets.push_back(mipmapped_texture_asset(
                            assets_writer,
                            names.unique(std::format("{}_metallic_roughness_texture", task.name)),
                            model.metallic_roughness_texture().value().get(),
                            GLTF_METALLIC_ROUGHNESS_TEXTURE_FORMAT,
                            AssetColorSpace::Linear
                        ));
                    }

                    // Materials.
                    {
                        assets.emplace_back(AssetMaterial {
                            .name = names.unique(std::format("{}_material", task.name)),
                            .alpha_cutoff = model.alpha_cutoff(),
                            .alpha_mode = (AssetAlphaMode)model.alpha_mode(),
                        });
                    }
                },
                [&](const AssetTaskProceduralCube& task) {
                    // Generate.
                    auto vertex_positions = std::vector<float3>();
                    auto vertex_normals = std::vector<float3>();
                    auto vertex_texcoords = std::vector<float2>();
                    auto indices = std::vector<uint>();
                    create_box(
                        vertex_positions,
                        vertex_normals,
                        vertex_texcoords,
                        indices,
                        {task.extents, task.extents, task.extents},
                        task.inverted,
                        task.inverted
                    );

                    // Compute tangents.
                    auto vertex_tangents = std::vector<float4>(vertex_positions.size());
                    generate_tangents(GenerateTangentsDesc {
                        .positions = vertex_positions,
                        .normals = vertex_normals,
                        .texcoords = vertex_texcoords,
                        .indices = indices,
                        .tangents = std::span(vertex_tangents),
                    });

                    // Convert.
                    auto vertices = std::vector<AssetVertex>(vertex_positions.size());
                    for (size_t i = 0; i < vertices.size(); ++i) {
                        vertices[i] = AssetVertex {
                            .position = vertex_positions[i],
                            .normal = vertex_normals[i],
                            .texcoord = vertex_texcoords[i],
                            .tangent = vertex_tangents[i],
                        };
                    }

                    // Submesh.
                    const auto submeshes = std::vector<AssetSubmesh> {
                        AssetSubmesh {
                            .index_count = (uint)indices.size(),
                            .start_index = 0,
                            .base_vertex = 0,
                        },
                    };

                    // Mesh.
                    assets.emplace_back(AssetMesh {
                        .name = names.unique(std::format("{}_mesh", task.name)),
                        .vertices =
                            assets_writer.write("Vertex", std::span<const AssetVertex>(vertices)),
                        .indices =
                            assets_writer.write("Index", std::span<const AssetIndex>(indices)),
                        .submeshes = assets_writer.write(
                            "Submesh",
                            std::span<const AssetSubmesh>(submeshes)
                        ),
                    });
                },
                [&](const AssetTaskProceduralSphere& task) {
                    // Generate.
                    auto vertex_positions = std::vector<float3>();
                    auto vertex_normals = std::vector<float3>();
                    auto vertex_texcoords = std::vector<float2>();
                    auto indices = std::vector<uint>();
                    create_sphere(
                        vertex_positions,
                        vertex_normals,
                        vertex_texcoords,
                        indices,
                        2.0f * task.radius,
                        task.tesselation,
                        task.inverted,
                        task.inverted
                    );

                    // Compute tangents.
                    auto vertex_tangents = std::vector<float4>(vertex_positions.size());
                    generate_tangents(GenerateTangentsDesc {
                        .positions = vertex_positions,
                        .normals = vertex_normals,
                        .texcoords = vertex_texcoords,
                        .indices = indices,
                        .tangents = std::span(vertex_tangents),
                    });

                    // Convert.
                    auto vertices = std::vector<AssetVertex>(vertex_positions.size());
                    for (size_t i = 0; i < vertices.size(); ++i) {
                        vertices[i] = AssetVertex {
                            .position = vertex_positions[i],
                            .normal = vertex_normals[i],
                            .texcoord = vertex_texcoords[i],
                            .tangent = vertex_tangents[i],
                        };
                    }

                    // Submesh.
                    const auto submeshes = std::vector<AssetSubmesh> {
                        AssetSubmesh {
                            .index_count = (uint)indices.size(),
                            .start_index = 0,
                            .base_vertex = 0,
                        },
                    };

                    // Mesh.
                    assets.emplace_back(AssetMesh {
                        .name = names.unique(std::format("{}_mesh", task.name)),
                        .vertices =
                            assets_writer.write("Vertex", std::span<const AssetVertex>(vertices)),
                        .indices =
                            assets_writer.write("Index", std::span<const AssetIndex>(indices)),
                        .submeshes = assets_writer.write(
                            "Submesh",
                            std::span<const AssetSubmesh>(submeshes)
                        ),
                    });
                },
                [&](const AssetTaskProceduralLowPolyGround& task) {
                    // Generate vertices.
                    const auto cell_count_x = task.vertex_count_x;
                    const auto cell_count_y = task.vertex_count_y;
                    const auto cell_vertex_count = cell_count_x * cell_count_y;
                    auto cell_vertices = std::vector<float3>(cell_vertex_count);
                    auto cell_vertices_visited = std::vector<bool>(cell_vertex_count, false);
                    const auto base_height = task.side_length * std::sqrtf(3.0f) / 2.0f;
                    for (uint cell_y = 0; cell_y < cell_count_y; cell_y++) {
                        for (uint cell_x = 0; cell_x < cell_count_x; cell_x++) {
                            const auto offset_x = cell_y % 2 == 1 ? 0.5f * task.side_length : 0.0f;

                            const auto cell_index = cell_y * cell_count_x + cell_x;
                            auto cell_position = float3();
                            cell_position.x = offset_x + (float)cell_x * task.side_length;
                            cell_position.y = 0.0f;
                            cell_position.z = (float)cell_y * base_height;
                            cell_vertices[cell_index] = cell_position;
                        }
                    }

                    // Re-center.
                    float3 cell_center = float3(0.0f, 0.0f, 0.0f);
                    for (const auto& v : cell_vertices) {
                        cell_center += v;
                    }
                    cell_center /= (float)cell_vertices.size();
                    for (auto& v : cell_vertices) {
                        v -= cell_center;
                    }

                    // Adjust heights.
                    Pcg rand;
                    for (auto& v : cell_vertices) {
                        v.y += task.height_variation * rand.random_float();
                    }

                    // Connect and push faces.
                    auto vertices = std::vector<AssetVertex>();
                    auto indices = std::vector<uint>();
                    const auto push_face = [&](uint a, uint b, uint c) {
                        const float3 p_a = cell_vertices[a];
                        const float3 p_b = cell_vertices[b];
                        const float3 p_c = cell_vertices[c];

                        const float3 d_ab = p_b - p_a;
                        const float3 d_ac = p_c - p_a;
                        const float3 n = float3_normalize(float3_cross(d_ab, d_ac));

                        AssetVertex v_a;
                        AssetVertex v_b;
                        AssetVertex v_c;

                        v_a.position = p_a;
                        v_b.position = p_b;
                        v_c.position = p_c;

                        v_a.normal = n;
                        v_b.normal = n;
                        v_c.normal = n;

                        v_a.texcoord = float2(0.5f, 0.5f);
                        v_b.texcoord = float2(0.5f, 0.5f);
                        v_c.texcoord = float2(0.5f, 0.5f);

                        v_a.tangent = float4(1.0f, 0.0f, 0.0f, 1.0f);
                        v_b.tangent = float4(1.0f, 0.0f, 0.0f, 1.0f);
                        v_c.tangent = float4(1.0f, 0.0f, 0.0f, 1.0f);

                        const uint i_a = (uint)vertices.size();
                        const uint i_b = i_a + 1;
                        const uint i_c = i_a + 2;

                        vertices.push_back(v_a);
                        vertices.push_back(v_b);
                        vertices.push_back(v_c);

                        indices.push_back(i_a);
                        indices.push_back(i_b);
                        indices.push_back(i_c);

                        cell_vertices_visited[a] = true;
                        cell_vertices_visited[b] = true;
                        cell_vertices_visited[c] = true;
                    };
                    for (uint j = 0; j < cell_count_y - 1; j++) {
                        for (uint i = 0; i < cell_count_x - 1; i++) {
                            const uint curr_offset_x = cell_count_x * j;
                            const uint next_offset_x = cell_count_x * (j + 1);
                            std::array<uint, 6> face_indices;
                            if (j % 2 == 0) {
                                face_indices[0] = curr_offset_x + i;
                                face_indices[1] = next_offset_x + i;
                                face_indices[2] = curr_offset_x + i + 1;
                                face_indices[3] = curr_offset_x + i + 1;
                                face_indices[4] = next_offset_x + i;
                                face_indices[5] = next_offset_x + i + 1;
                            } else {
                                face_indices[0] = curr_offset_x + i;
                                face_indices[1] = next_offset_x + i;
                                face_indices[2] = next_offset_x + i + 1;
                                face_indices[3] = curr_offset_x + i;
                                face_indices[4] = next_offset_x + i + 1;
                                face_indices[5] = curr_offset_x + i + 1;
                            }
                            push_face(face_indices[0], face_indices[1], face_indices[2]);
                            push_face(face_indices[3], face_indices[4], face_indices[5]);
                        }
                    }

                    // Verify all vertices were visited.
                    uint visited_count = 0;
                    for (uint i = 0; i < cell_vertices.size(); i++) {
                        if (cell_vertices_visited[i]) {
                            visited_count++;
                        } else {
                            FB_LOG_INFO(
                                "Was not visited: {}: {}",
                                i,
                                (bool)cell_vertices_visited[i]
                            );
                        }
                    }
                    FB_ASSERT(visited_count == cell_vertices.size());

                    // Submesh.
                    const auto submeshes = std::vector<AssetSubmesh> {
                        AssetSubmesh {
                            .index_count = (uint)indices.size(),
                            .start_index = 0,
                            .base_vertex = 0,
                        },
                    };

                    // Mesh.
                    assets.emplace_back(AssetMesh {
                        .name = names.unique(std::format("{}_mesh", task.name)),
                        .vertices =
                            assets_writer.write("Vertex", std::span<const AssetVertex>(vertices)),
                        .indices =
                            assets_writer.write("Index", std::span<const AssetIndex>(indices)),
                        .submeshes = assets_writer.write(
                            "Submesh",
                            std::span<const AssetSubmesh>(submeshes)
                        ),
                    });
                },
                [&](const AssetTaskStockcubeOutput& task) {
                    const auto bin_path = std::format("{}/{}", assets_dir, task.bin_path);
                    const auto bin_bytes = read_whole_file(bin_path);
                    const auto bin_span = std::span<const std::byte>(bin_bytes);

                    const auto json_path = std::format("{}/{}", assets_dir, task.json_path);
                    const auto json_bytes = read_whole_file(json_path);
                    const auto json = json::parse(json_bytes);
                    const auto format = (DXGI_FORMAT)json["format"].template get<uint>();
                    const auto unit_byte_count = json["unit_byte_count"].template get<uint>();
                    const auto width = json["width"].template get<uint>();
                    const auto height = json["height"].template get<uint>();
                    const auto depth = json["depth"].template get<uint>();
                    const auto mip_count = json["mip_count"].template get<uint>();
                    FB_ASSERT(mip_count <= MAX_MIP_COUNT);

                    if (depth == 6) {
                        std::array<std::array<AssetTextureData, MAX_MIP_COUNT>, 6> texture_datas = {
                        };
                        uint64_t offset = 0;
                        for (uint slice = 0; slice < depth; slice++) {
                            auto& slice_datas = texture_datas[slice];
                            for (uint mip = 0; mip < mip_count; mip++) {
                                const auto mip_width = std::max(1u, width >> mip);
                                const auto mip_height = std::max(1u, height >> mip);
                                const auto row_pitch = mip_width * unit_byte_count;
                                const auto slice_pitch = row_pitch * mip_height;
                                slice_datas[mip] = AssetTextureData {
                                    .row_pitch = row_pitch,
                                    .slice_pitch = slice_pitch,
                                    .data = assets_writer.write(
                                        "std::byte",
                                        bin_span.subspan(offset, slice_pitch)
                                    ),
                                };
                                offset += slice_pitch;
                            }
                        }

                        assets.emplace_back(AssetCubeTexture {
                            .name = names.unique(std::string(task.name)),
                            .format = format,
                            .width = width,
                            .height = height,
                            .mip_count = mip_count,
                            .datas = texture_datas,
                        });
                    } else {
                        const auto row_pitch = width * unit_byte_count;
                        const auto slice_pitch = row_pitch * height;
                        assets.emplace_back(AssetTexture {
                            .name = std::string(task.name),
                            .format = format,
                            .width = width,
                            .height = height,
                            .mip_count = mip_count,
                            .datas = {AssetTextureData {
                                .row_pitch = row_pitch,
                                .slice_pitch = slice_pitch,
                                .data = assets_writer.write("std::byte", bin_span),
                            }},
                        });
                    }
                },
                [&](const AssetTaskTtf& task) {
                    const auto path = std::format("{}/{}", assets_dir, task.path);
                    const auto file = read_whole_file(path);

                    int ttf_result;
                    ttf_t* ttf = nullptr;
                    ttf_result = ttf_load_from_mem(
                        (const uint8_t*)file.data(),
                        (int)file.size(),
                        &ttf,
                        false
                    );
                    FB_ASSERT_MSG(
                        ttf_result == TTF_DONE,
                        "Failed to load TTF file with error {}",
                        ttf_result
                    );

                    std::vector<AssetGlyph> glyphs;
                    std::vector<AssetSubmesh> submeshes;
                    std::vector<AssetVertex> vertices;
                    std::vector<AssetIndex> indices;
                    size_t vertices_offset = 0;
                    size_t indices_offset = 0;

                    for (char c = '!'; c <= '~'; c++) {
                        // Find glyph.
                        const auto glyph_id = ttf_find_glyph(ttf, c);
                        FB_ASSERT(glyph_id != -1);
                        const auto glyph = &ttf->glyphs[glyph_id];

                        // Push glyph info.
                        glyphs.push_back(AssetGlyph {
                            .character = (uint)c,
                            .xbounds = float2(glyph->xbounds[0], glyph->xbounds[1]),
                            .ybounds = float2(glyph->ybounds[0], glyph->ybounds[1]),
                            .advance = glyph->advance,
                            .lbearing = glyph->lbearing,
                            .rbearing = glyph->rbearing,
                        });

                        // Generate mesh.
                        const auto quality = TTF_QUALITY_HIGH;
                        const auto features = TTF_FEATURES_DFLT;
                        const auto depth = task.depth;
                        ttf_mesh3d_t* glyph_mesh = nullptr;
                        ttf_result = ttf_glyph2mesh3d(glyph, &glyph_mesh, quality, features, depth);
                        FB_ASSERT_MSG(
                            ttf_result == TTF_DONE,
                            "Failed to create glyph mesh with error {}",
                            ttf_result
                        );

                        // Copy vertices.
                        const auto base_vertex = (uint)vertices.size();
                        vertices.resize(vertices.size() + glyph_mesh->nvert);
                        for (int i = 0; i < glyph_mesh->nvert; i++) {
                            const auto& v = glyph_mesh->vert[i];
                            const auto& n = glyph_mesh->normals[i];
                            vertices[vertices_offset++] = AssetVertex {
                                .position = float3(v.x, v.y, v.z),
                                .normal = float3(n.x, n.y, n.z),
                                .texcoord = float2(0.0f, 0.0f),
                                .tangent = float4(0.0f, 0.0f, 0.0f, 0.0f),
                            };
                        }

                        // Copy indices.
                        const auto start_index = (uint)indices.size();
                        indices.resize(indices.size() + glyph_mesh->nfaces * 3);
                        for (int i = 0; i < glyph_mesh->nfaces; i++) {
                            const auto& face = glyph_mesh->faces[i];
                            indices[indices_offset++] = (AssetIndex)face.v1;
                            indices[indices_offset++] = (AssetIndex)face.v2;
                            indices[indices_offset++] = (AssetIndex)face.v3;
                        }

                        // Submesh.
                        submeshes.push_back(AssetSubmesh {
                            .index_count = (uint)glyph_mesh->nfaces * 3,
                            .start_index = start_index,
                            .base_vertex = base_vertex,
                        });

                        // Free mesh.
                        ttf_free_mesh3d(glyph_mesh);
                    }

                    // Find space glyph.
                    const auto space_glyph_id = ttf_find_glyph(ttf, ' ');
                    FB_ASSERT(space_glyph_id != -1);
                    const auto space_glyph = &ttf->glyphs[space_glyph_id];

                    // Push assets.
                    assets.emplace_back(AssetFont {
                        .name = names.unique(std::format("{}_font", task.name)),
                        .ascender = ttf->hhea.ascender,
                        .descender = ttf->hhea.descender,
                        .space_advance = space_glyph->advance,
                        .glyphs = assets_writer.write("Glyph", std::span<const AssetGlyph>(glyphs)),
                    });
                    assets.emplace_back(AssetMesh {
                        .name = names.unique(std::format("{}_mesh", task.name)),
                        .vertices =
                            assets_writer.write("Vertex", std::span<const AssetVertex>(vertices)),
                        .indices =
                            assets_writer.write("Index", std::span<const AssetIndex>(indices)),
                        .submeshes = assets_writer.write(
                            "Submesh",
                            std::span<const AssetSubmesh>(submeshes)
                        ),
                    });

                    // Cleanup.
                    ttf_free(ttf);
                }
            },
            asset_task
        );
    }

    return {assets, assets_bin};
}

} // namespace fb
