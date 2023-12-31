#include "tasks.hpp"
#include "../formats/gltf.hpp"
#include "../formats/mikktspace.hpp"
#include "../utils/names.hpp"

#include <ttf2mesh.h>
#include <stb_image_resize.h>
#include <directxtk12/GeometricPrimitive.h>
#include <nlohmann/json.hpp>
using json = nlohmann::json;
using ord_json = nlohmann::ordered_json;

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
    dst_buffer.assign(texture.data().begin(), texture.data().end());
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
                    const auto path = std::format("{}/{}", assets_dir, task.path);
                    const auto file = read_whole_file(path);
                    assets.push_back(AssetCopy {
                        .name = names.unique(std::string(task.name)),
                        .data = assets_writer.write("std::byte", std::span(file)),
                    });
                },
                [&](const AssetTaskTexture& task) {
                    const auto path = std::format("{}/{}", assets_dir, task.path);
                    const auto file = read_whole_file(path);
                    const auto image = LdrImage::load(file);
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
                    const auto image = HdrImage::load(file);
                    assets.push_back(AssetTexture {
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
                    auto tangents = std::vector<float4>(positions.size());
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

                        assets.push_back(AssetMesh {
                            .name = names.unique(std::format("{}_mesh", task.name)),
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

                        assets.push_back(AssetAnimationMesh {
                            .name = names.unique(std::format("{}_animation_mesh", task.name)),
                            .node_count = model.node_count(),
                            .joint_count = model.joint_count(),
                            .duration = model.animation_duration(),
                            .skinning_vertices = assets_writer.write(
                                "SkinningVertex",
                                std::span<const AssetSkinningVertex>(vertices)
                            ),
                            .indices = assets_writer.write("Index", indices),
                            .joint_nodes = assets_writer.write("uint", model.joint_nodes()),
                            .joint_inverse_binds =
                                assets_writer.write("float4x4", model.joint_inverse_binds()),
                            .node_parents = assets_writer.write("uint", model.node_parents()),
                            .node_transforms =
                                assets_writer.write("float4x4", model.node_transforms()),
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
                                assets_writer.write("Quaternion", model.node_channels_values_r()),
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
                    auto vertex_positions = std::vector<float3>(dxtk_vertices.size());
                    auto vertex_normals = std::vector<float3>(dxtk_vertices.size());
                    auto vertex_texcoords = std::vector<float2>(dxtk_vertices.size());
                    for (auto i = 0; i < dxtk_vertices.size(); ++i) {
                        vertex_positions[i] = dxtk_vertices[i].position;
                        vertex_normals[i] = dxtk_vertices[i].normal;
                        vertex_texcoords[i] = dxtk_vertices[i].textureCoordinate;
                    }

                    // Convert indices.
                    auto indices = std::vector<uint>(dxtk_indices.size());
                    for (auto i = 0; i < dxtk_indices.size(); ++i) {
                        indices[i] = (uint)dxtk_indices[i];
                    }

                    // Compute tangents.
                    auto vertex_tangents = std::vector<float4>(dxtk_vertices.size());
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
                    assets.push_back(AssetMesh {
                        .name = names.unique(std::format("{}_mesh", task.name)),
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
                    auto vertex_positions = std::vector<float3>(dxtk_vertices.size());
                    auto vertex_normals = std::vector<float3>(dxtk_vertices.size());
                    auto vertex_texcoords = std::vector<float2>(dxtk_vertices.size());
                    for (auto i = 0; i < dxtk_vertices.size(); ++i) {
                        vertex_positions[i] = dxtk_vertices[i].position;
                        vertex_normals[i] = dxtk_vertices[i].normal;
                        vertex_texcoords[i] = dxtk_vertices[i].textureCoordinate;
                    }

                    // Convert indices.
                    auto indices = std::vector<uint>(dxtk_indices.size());
                    for (auto i = 0; i < dxtk_indices.size(); ++i) {
                        indices[i] = (uint)dxtk_indices[i];
                    }

                    // Compute tangents.
                    auto vertex_tangents = std::vector<float4>(dxtk_vertices.size());
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
                    assets.push_back(AssetMesh {
                        .name = names.unique(std::format("{}_mesh", task.name)),
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

                        assets.push_back(AssetCubeTexture {
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
                    assets.push_back(AssetFont {
                        .name = names.unique(std::format("{}_font", task.name)),
                        .ascender = ttf->hhea.ascender,
                        .descender = ttf->hhea.descender,
                        .space_advance = space_glyph->advance,
                        .glyphs = assets_writer.write("Glyph", std::span<const AssetGlyph>(glyphs)),
                    });
                    assets.push_back(AssetMeshArray {
                        .name = names.unique(std::format("{}_mesh_array", task.name)),
                        .submeshes = assets_writer.write(
                            "Submesh",
                            std::span<const AssetSubmesh>(submeshes)
                        ),
                        .vertices =
                            assets_writer.write("Vertex", std::span<const AssetVertex>(vertices)),
                        .indices =
                            assets_writer.write("Index", std::span<const AssetIndex>(indices)),
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
