#include "tasks.hpp"
#include "../formats/gltf.hpp"
#include "../formats/mikktspace.hpp"
#include "../utils/names.hpp"
#include <stb_image_resize.h>

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
    uint32_t texture_data_count = 0;
    const uint32_t mip_count = mip_count_from_size(texture.size());
    FB_ASSERT(mip_count <= MAX_MIP_COUNT);
    uint32_t src_width = 0;
    uint32_t src_height = 0;
    std::vector<std::byte> src_buffer;
    uint32_t dst_width = 0;
    uint32_t dst_height = 0;
    std::vector<std::byte> dst_buffer;

    // First mip can simply be copied.
    dst_width = texture.width();
    dst_height = texture.height();
    dst_buffer.assign(texture.data().begin(), texture.data().end());
    texture_datas[texture_data_count++] = AssetTextureData {
        .row_pitch = dst_width * texture.channel_count(),
        .slice_pitch = dst_width * dst_height * texture.channel_count(),
        .data = assets_writer.write("std::byte", std::span<const std::byte>(dst_buffer)),
    };

    // Compute the rest of the mip levels.
    for (uint32_t mip = 1; mip < mip_count; mip++) {
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
                (int)texture.channel_count(),
                3,
                0
            );
        } else {
            stbir_resize_uint8(
                (const uint8_t*)src_buffer.data(),
                (int)src_width,
                (int)src_height,
                (int)(src_width * texture.channel_count()),
                (uint8_t*)dst_buffer.data(),
                (int)dst_width,
                (int)dst_height,
                (int)(dst_width * texture.channel_count()),
                (int)texture.channel_count()
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
                    const auto name = names.unique(std::string(task.name));
                    const auto path = std::format("{}/{}", assets_dir, task.path);
                    const auto file = read_whole_file(path);
                    assets.push_back(AssetCopy {
                        .name = name,
                        .data = assets_writer.write("std::byte", std::span(file)),
                    });
                },
                [&](const AssetTaskTexture& task) {
                    const auto name = names.unique(std::format("{}_texture", task.name));
                    const auto path = std::format("{}/{}", assets_dir, task.path);
                    const auto file = read_whole_file(path);
                    const auto image = LdrImage::load(file);
                    assets.push_back(mipmapped_texture_asset(
                        assets_writer,
                        name,
                        image,
                        task.format,
                        task.color_space
                    ));
                },
                [&](const AssetTaskHdrTexture& task) {
                    const auto name = names.unique(std::format("{}_hdr_texture", task.name));
                    const auto file = read_whole_file(std::format("{}/{}", assets_dir, task.path));
                    const auto image = HdrImage::load(file);
                    assets.push_back(AssetTexture {
                        .name = name,
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
                    auto tangents = std::vector<Float4>(positions.size());
                    generate_tangents(GenerateTangentsDesc {
                        .positions = positions,
                        .normals = normals,
                        .texcoords = texcoords,
                        .indices = indices,
                        .tangents = std::span(tangents),
                    });

                    // Animated vs non-animated.
                    if (joints.empty()) {
                        auto vertices = std::vector<AssetVertex>(positions.size());
                        for (auto i = 0; i < vertices.size(); ++i) {
                            vertices[i] = AssetVertex {
                                .position = positions[i],
                                .normal = normals[i],
                                .texcoord = texcoords[i],
                                .tangent = tangents[i],
                            };
                        }

                        const auto mesh_name = names.unique(std::format("{}_mesh", task.name));
                        assets.push_back(AssetMesh {
                            .name = mesh_name,
                            .vertices = assets_writer.write(
                                "Vertex",
                                std::span<const AssetVertex>(vertices)
                            ),
                            .indices = assets_writer.write("Index", indices),
                        });
                    } else {
                        auto vertices = std::vector<AssetSkinningVertex>(positions.size());
                        for (auto i = 0; i < vertices.size(); ++i) {
                            vertices[i] = AssetSkinningVertex {
                                .position = positions[i],
                                .normal = normals[i],
                                .texcoord = texcoords[i],
                                .tangent = tangents[i],
                                .joint = joints[i],
                                .weight = weights[i],
                            };
                        }

                        const auto mesh_name =
                            names.unique(std::format("{}_animation_mesh", task.name));
                        assets.push_back(AssetAnimationMesh {
                            .name = mesh_name,
                            .node_count = model.node_count(),
                            .joint_count = model.joint_count(),
                            .duration = model.animation_duration(),
                            .skinning_vertices = assets_writer.write(
                                "SkinningVertex",
                                std::span<const AssetSkinningVertex>(vertices)
                            ),
                            .indices = assets_writer.write("Index", indices),
                            .joint_nodes = assets_writer.write("uint32_t", model.joint_nodes()),
                            .joint_inverse_binds =
                                assets_writer.write("Float4x4", model.joint_inverse_binds()),
                            .node_parents = assets_writer.write("uint32_t", model.node_parents()),
                            .node_transforms =
                                assets_writer.write("Float4x4", model.node_transforms()),
                            .node_channels =
                                assets_writer.write("AnimationChannel", model.node_channels()),
                            .node_channels_times_t =
                                assets_writer.write("float", model.node_channels_times_t()),
                            .node_channels_times_r =
                                assets_writer.write("float", model.node_channels_times_r()),
                            .node_channels_times_s =
                                assets_writer.write("float", model.node_channels_times_s()),
                            .node_channels_values_t =
                                assets_writer.write("Float3", model.node_channels_values_t()),
                            .node_channels_values_r =
                                assets_writer.write("Quaternion", model.node_channels_values_r()),
                            .node_channels_values_s =
                                assets_writer.write("Float3", model.node_channels_values_s()),
                        });
                    }

                    // Textures.
                    {
                        const auto texture = model.base_color_texture();
                        const auto texture_name =
                            names.unique(std::format("{}_base_color_texture", task.name));
                        assets.push_back(mipmapped_texture_asset(
                            assets_writer,
                            texture_name,
                            texture,
                            GLTF_BASE_COLOR_TEXTURE_FORMAT,
                            AssetColorSpace::Srgb
                        ));
                    }
                    if (model.normal_texture().has_value()) {
                        const auto texture = model.normal_texture().value().get();
                        const auto texture_name =
                            names.unique(std::format("{}_normal_texture", task.name));
                        assets.push_back(mipmapped_texture_asset(
                            assets_writer,
                            texture_name,
                            texture,
                            GLTF_NORMAL_TEXTURE_FORMAT,
                            AssetColorSpace::Linear
                        ));
                    }
                    if (model.metallic_roughness_texture().has_value()) {
                        const auto texture = model.metallic_roughness_texture().value().get();
                        const auto texture_name =
                            names.unique(std::format("{}_metallic_roughness_texture", task.name));
                        assets.push_back(mipmapped_texture_asset(
                            assets_writer,
                            texture_name,
                            texture,
                            GLTF_METALLIC_ROUGHNESS_TEXTURE_FORMAT,
                            AssetColorSpace::Linear
                        ));
                    }
                },
                [&](const AssetTaskProceduralCube& task) {
                    // Generate.
                    using namespace DirectX::DX12;
                    using Vertices = GeometricPrimitive::VertexCollection;
                    using Indices = GeometricPrimitive::IndexCollection;
                    Vertices dxtk_vertices;
                    Indices dxtk_indices;
                    GeometricPrimitive::CreateBox(
                        dxtk_vertices,
                        dxtk_indices,
                        {task.extents, task.extents, task.extents},
                        task.inverted,
                        task.inverted
                    );

                    // Flatten vertex attributes.
                    auto vertex_positions = std::vector<Float3>(dxtk_vertices.size());
                    auto vertex_normals = std::vector<Float3>(dxtk_vertices.size());
                    auto vertex_texcoords = std::vector<Float2>(dxtk_vertices.size());
                    for (auto i = 0; i < dxtk_vertices.size(); ++i) {
                        vertex_positions[i] = dxtk_vertices[i].position;
                        vertex_normals[i] = dxtk_vertices[i].normal;
                        vertex_texcoords[i] = dxtk_vertices[i].textureCoordinate;
                    }

                    // Convert indices.
                    auto indices = std::vector<uint32_t>(dxtk_indices.size());
                    for (auto i = 0; i < dxtk_indices.size(); ++i) {
                        indices[i] = (uint32_t)dxtk_indices[i];
                    }

                    // Compute tangents.
                    auto vertex_tangents = std::vector<Float4>(dxtk_vertices.size());
                    generate_tangents(GenerateTangentsDesc {
                        .positions = vertex_positions,
                        .normals = vertex_normals,
                        .texcoords = vertex_texcoords,
                        .indices = indices,
                        .tangents = std::span(vertex_tangents),
                    });

                    // Convert.
                    auto vertices = std::vector<AssetVertex>(dxtk_vertices.size());
                    for (auto i = 0; i < vertices.size(); ++i) {
                        vertices[i] = AssetVertex {
                            .position = vertex_positions[i],
                            .normal = vertex_normals[i],
                            .texcoord = vertex_texcoords[i],
                            .tangent = vertex_tangents[i],
                        };
                    }

                    // Mesh.
                    const auto mesh_name = names.unique(std::format("{}_mesh", task.name));
                    assets.push_back(AssetMesh {
                        .name = mesh_name,
                        .vertices =
                            assets_writer.write("Vertex", std::span<const AssetVertex>(vertices)),
                        .indices =
                            assets_writer.write("Index", std::span<const AssetIndex>(indices)),
                    });
                },
                [&](const AssetTaskProceduralSphere& task) {
                    // Generate.
                    using namespace DirectX::DX12;
                    using Vertices = GeometricPrimitive::VertexCollection;
                    using Indices = GeometricPrimitive::IndexCollection;
                    Vertices dxtk_vertices;
                    Indices dxtk_indices;
                    GeometricPrimitive::CreateSphere(
                        dxtk_vertices,
                        dxtk_indices,
                        2.0f * task.radius,
                        task.tesselation,
                        task.inverted,
                        task.inverted
                    );

                    // Flatten vertex attributes.
                    auto vertex_positions = std::vector<Float3>(dxtk_vertices.size());
                    auto vertex_normals = std::vector<Float3>(dxtk_vertices.size());
                    auto vertex_texcoords = std::vector<Float2>(dxtk_vertices.size());
                    for (auto i = 0; i < dxtk_vertices.size(); ++i) {
                        vertex_positions[i] = dxtk_vertices[i].position;
                        vertex_normals[i] = dxtk_vertices[i].normal;
                        vertex_texcoords[i] = dxtk_vertices[i].textureCoordinate;
                    }

                    // Convert indices.
                    auto indices = std::vector<uint32_t>(dxtk_indices.size());
                    for (auto i = 0; i < dxtk_indices.size(); ++i) {
                        indices[i] = (uint32_t)dxtk_indices[i];
                    }

                    // Compute tangents.
                    auto vertex_tangents = std::vector<Float4>(dxtk_vertices.size());
                    generate_tangents(GenerateTangentsDesc {
                        .positions = vertex_positions,
                        .normals = vertex_normals,
                        .texcoords = vertex_texcoords,
                        .indices = indices,
                        .tangents = std::span(vertex_tangents),
                    });

                    // Convert.
                    auto vertices = std::vector<AssetVertex>(dxtk_vertices.size());
                    for (auto i = 0; i < vertices.size(); ++i) {
                        vertices[i] = AssetVertex {
                            .position = vertex_positions[i],
                            .normal = vertex_normals[i],
                            .texcoord = vertex_texcoords[i],
                            .tangent = vertex_tangents[i],
                        };
                    }

                    // Mesh.
                    const auto mesh_name = names.unique(std::format("{}_mesh", task.name));
                    assets.push_back(AssetMesh {
                        .name = mesh_name,
                        .vertices =
                            assets_writer.write("Vertex", std::span<const AssetVertex>(vertices)),
                        .indices =
                            assets_writer.write("Index", std::span<const AssetIndex>(indices)),
                    });
                },
                [&](const AssetTaskStockcubeOutput& task) {
                    const auto bin_path = std::format("{}/{}", assets_dir, task.bin_path);
                    const auto bin_bytes = read_whole_file(bin_path);
                    const auto bin_span = std::span<const std::byte>(bin_bytes);

                    const auto json_path = std::format("{}/{}", assets_dir, task.json_path);
                    const auto json_bytes = read_whole_file(json_path);
                    const auto json = json::parse(json_bytes);
                    const auto format = (DXGI_FORMAT)json["format"].template get<uint32_t>();
                    const auto unit_byte_count = json["unit_byte_count"].template get<uint32_t>();
                    const auto width = json["width"].template get<uint32_t>();
                    const auto height = json["height"].template get<uint32_t>();
                    const auto depth = json["depth"].template get<uint32_t>();
                    const auto mip_count = json["mip_count"].template get<uint32_t>();
                    FB_ASSERT(mip_count <= MAX_MIP_COUNT);

                    if (depth == 6) {
                        std::array<std::array<AssetTextureData, MAX_MIP_COUNT>, 6> texture_datas =
                            {};
                        uint64_t offset = 0;
                        for (uint32_t slice = 0; slice < depth; slice++) {
                            auto& slice_datas = texture_datas[slice];
                            for (uint32_t mip = 0; mip < mip_count; mip++) {
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

                        assets.push_back(AssetCubeTexture {
                            .name = std::string(task.name),
                            .format = format,
                            .width = width,
                            .height = height,
                            .mip_count = mip_count,
                            .datas = texture_datas,
                        });
                    } else {
                        const auto row_pitch = width * unit_byte_count;
                        const auto slice_pitch = row_pitch * height;
                        assets.push_back(AssetTexture {
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
                }},
            asset_task
        );
    }

    return {assets, assets_bin};
}

} // namespace fb
