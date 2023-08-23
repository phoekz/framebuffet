#include <common/common.hpp>
#include "shaders/shaders.hpp"
#include "formats/gltf.hpp"
#include "formats/exr.hpp"
#include "formats/mikktspace.hpp"

namespace fb {

//
// Asset tasks.
//

struct AssetTaskCopy {
    std::string_view name;
    std::string_view path;
};

struct AssetTaskTexture {
    std::string_view name;
    std::string_view path;
};

struct AssetTaskCubeTexture {
    std::string_view name;
    std::array<std::string_view, 6> paths;
};

struct AssetTaskGltf {
    std::string_view name;
    std::string_view path;
};

struct AssetTaskProceduralCube {
    std::string_view name;
    float extents;
    bool inverted;
};

using AssetTask = std::variant<
    AssetTaskCopy,
    AssetTaskTexture,
    AssetTaskCubeTexture,
    AssetTaskGltf,
    AssetTaskProceduralCube>;

static auto asset_tasks = std::to_array<AssetTask>({
    AssetTaskCopy {"imgui_font", "fonts/Roboto-Medium.ttf"},
    AssetTaskTexture {"heatmap_magma", "heatmaps/magma.png"},
    AssetTaskTexture {"heatmap_viridis", "heatmaps/viridis.png"},
    AssetTaskGltf {"sci_fi_case", "models/sci_fi_case.glb"},
    AssetTaskGltf {"metal_plane", "models/metal_plane.glb"},
    AssetTaskGltf {"coconut_tree", "models/coconut_tree.glb"},
    AssetTaskGltf {"sand_plane", "models/sand_plane.glb"},
    AssetTaskGltf {"raccoon", "models/low-poly_racoon_run_animation.glb"},
    AssetTaskProceduralCube {"light_bounds", 2.0f, false},
    AssetTaskProceduralCube {"skybox", 2.0f, true},
    AssetTaskCubeTexture {
        "winter_evening",
        std::to_array({
            "envmaps/winter_evening_1k_+X.exr"sv,
            "envmaps/winter_evening_1k_-X.exr"sv,
            "envmaps/winter_evening_1k_+Y.exr"sv,
            "envmaps/winter_evening_1k_-Y.exr"sv,
            "envmaps/winter_evening_1k_+Z.exr"sv,
            "envmaps/winter_evening_1k_-Z.exr"sv,
        })},
});

//
// Assets.
//

struct AssetSpan {
    std::string type;
    size_t offset;
    size_t element_size;
    size_t byte_size;
};

struct AssetCopy {
    std::string name;
    AssetSpan data;
};

struct AssetVertex {
    Float3 position;
    Float3 normal;
    Float2 texcoord;
    Float4 tangent;
};

struct AssetSkinningVertex {
    Float3 position;
    Float3 normal;
    Float2 texcoord;
    Float4 tangent;
    Uint4 joint;
    Float4 weight;
};

using AssetIndex = uint32_t;

struct AssetMesh {
    std::string name;

    AssetSpan vertices;
    AssetSpan indices;
};

struct AssetTexture {
    std::string name;

    DXGI_FORMAT format;
    uint32_t width;
    uint32_t height;
    uint32_t channels;
    uint32_t row_pitch;
    uint32_t slice_pitch;
    AssetSpan data;
};

struct AssetCubeTexture {
    std::string name;

    DXGI_FORMAT format;
    uint32_t width;
    uint32_t height;
    uint32_t channels;
    uint32_t row_pitch;
    uint32_t slice_pitch;
    std::array<AssetSpan, 6> datas;
};

struct AssetAnimationMesh {
    std::string name;

    uint32_t node_count;
    uint32_t joint_count;
    float duration;

    AssetSpan skinning_vertices;
    AssetSpan indices;
    AssetSpan joint_nodes;
    AssetSpan joint_inverse_binds;
    AssetSpan node_parents;
    AssetSpan node_transforms;
    AssetSpan node_channels;
    AssetSpan node_channels_times_t;
    AssetSpan node_channels_times_r;
    AssetSpan node_channels_times_s;
    AssetSpan node_channels_values_t;
    AssetSpan node_channels_values_r;
    AssetSpan node_channels_values_s;
};

using Asset =
    std::variant<AssetCopy, AssetMesh, AssetTexture, AssetCubeTexture, AssetAnimationMesh>;

//
// Asset builder.
//

class AssetWriter {
public:
    AssetWriter(std::vector<std::byte>& data)
        : _data(data) {}

    template<typename T>
    auto write(std::string_view type, std::span<const T> elements) -> AssetSpan {
        auto offset = _data.size();
        _data.resize(offset + elements.size_bytes());
        std::memcpy(_data.data() + offset, elements.data(), elements.size_bytes());
        return {
            .type = std::string(type),
            .offset = offset,
            .element_size = elements.size(),
            .byte_size = elements.size_bytes(),
        };
    }

private:
    std::vector<std::byte>& _data;
};

auto build_assets(std::string_view assets_dir)
    -> std::tuple<std::vector<Asset>, std::vector<std::byte>> {
    auto assets = std::vector<Asset>();
    auto assets_bin = std::vector<std::byte>();
    auto assets_writer = AssetWriter(assets_bin);
    auto unique_names = std::unordered_set<std::string>();
    auto update_unique_names = [](std::unordered_set<std::string>& uniques,
                                  const std::string& name) {
        if (uniques.contains(name)) {
            FB_LOG_ERROR("Duplicate asset name: {}", name);
            FB_FATAL();
        }
        uniques.insert(name);
    };
    for (const auto& asset_task : asset_tasks) {
        std::visit(
            overloaded {
                [&](const AssetTaskCopy& task) {
                    const auto name = std::string(task.name);
                    update_unique_names(unique_names, name);
                    const auto path = std::format("{}/{}", assets_dir, task.path);
                    const auto file = read_whole_file(path);
                    assets.push_back(AssetCopy {
                        .name = name,
                        .data = assets_writer.write("std::byte", std::span(file)),
                    });
                },
                [&](const AssetTaskTexture& task) {
                    const auto name = std::format("{}_texture", task.name);
                    update_unique_names(unique_names, name);
                    const auto path = std::format("{}/{}", assets_dir, task.path);
                    const auto file = read_whole_file(path);
                    const auto image = Image::load(file);
                    assets.push_back(AssetTexture {
                        .name = name,
                        .format = GLTF_BASE_COLOR_TEXTURE_FORMAT,
                        .width = image.width(),
                        .height = image.height(),
                        .channels = image.channels(),
                        .row_pitch = image.row_pitch(),
                        .slice_pitch = image.slice_pitch(),
                        .data = assets_writer.write("std::byte", std::span(image.data())),
                    });
                },
                [&](const AssetTaskCubeTexture& task) {
                    // Name.
                    const auto name = std::format("{}_cube_texture", task.name);
                    update_unique_names(unique_names, name);

                    // Load faces.
                    auto cube_faces = std::to_array({
                        ExrImage::load(std::format("{}/{}", assets_dir, task.paths[0])), // PosX
                        ExrImage::load(std::format("{}/{}", assets_dir, task.paths[1])), // NegX
                        ExrImage::load(std::format("{}/{}", assets_dir, task.paths[2])), // PosY
                        ExrImage::load(std::format("{}/{}", assets_dir, task.paths[3])), // NegY
                        ExrImage::load(std::format("{}/{}", assets_dir, task.paths[4])), // PosZ
                        ExrImage::load(std::format("{}/{}", assets_dir, task.paths[5])), // NegZ
                    });

                    // Swap Z faces to match DirectX convention.
                    std::swap(cube_faces[4], cube_faces[5]);

                    // Validate.
                    auto cube_face_size = cube_faces[0].size();
                    auto cube_format = cube_faces[0].format();
                    FB_ASSERT(std::ranges::all_of(cube_faces, [&](const auto& face) {
                        return face.width() == cube_face_size.x && face.height() == cube_face_size.y
                            && face.format() == cube_format;
                    }));

                    // Write.
                    std::array<AssetSpan, 6> datas;
                    for (auto i = 0; i < 6; ++i) {
                        datas[i] = assets_writer.write("float", cube_faces[i].data());
                    }
                    assets.push_back(AssetCubeTexture {
                        .name = name,
                        .format = cube_format,
                        .width = cube_face_size.x,
                        .height = cube_face_size.y,
                        .channels = cube_faces[0].channels(),
                        .row_pitch = cube_faces[0].row_pitch(),
                        .slice_pitch = cube_faces[0].slice_pitch(),
                        .datas = datas,
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

                        const auto mesh_name = std::format("{}_mesh", task.name);
                        update_unique_names(unique_names, mesh_name);
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

                        const auto mesh_name = std::format("{}_animation_mesh", task.name);
                        update_unique_names(unique_names, mesh_name);
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
                        const auto texture_name = std::format("{}_base_color_texture", task.name);
                        update_unique_names(unique_names, texture_name);
                        assets.push_back(AssetTexture {
                            .name = texture_name,
                            .format = GLTF_BASE_COLOR_TEXTURE_FORMAT,
                            .width = texture.width(),
                            .height = texture.height(),
                            .channels = texture.channels(),
                            .row_pitch = texture.row_pitch(),
                            .slice_pitch = texture.slice_pitch(),
                            .data = assets_writer.write("std::byte", texture.data()),
                        });
                    }
                    if (model.normal_texture().has_value()) {
                        const auto texture = model.normal_texture().value().get();
                        const auto texture_name = std::format("{}_normal_texture", task.name);
                        update_unique_names(unique_names, texture_name);
                        assets.push_back(AssetTexture {
                            .name = texture_name,
                            .format = GLTF_NORMAL_TEXTURE_FORMAT,
                            .width = texture.width(),
                            .height = texture.height(),
                            .channels = texture.channels(),
                            .row_pitch = texture.row_pitch(),
                            .slice_pitch = texture.slice_pitch(),
                            .data = assets_writer.write("std::byte", texture.data()),
                        });
                    }
                    if (model.metallic_roughness_texture().has_value()) {
                        const auto texture = model.metallic_roughness_texture().value().get();
                        const auto texture_name =
                            std::format("{}_metallic_roughness_texture", task.name);
                        update_unique_names(unique_names, texture_name);
                        assets.push_back(AssetTexture {
                            .name = texture_name,
                            .format = GLTF_METALLIC_ROUGHNESS_TEXTURE_FORMAT,
                            .width = texture.width(),
                            .height = texture.height(),
                            .channels = texture.channels(),
                            .row_pitch = texture.row_pitch(),
                            .slice_pitch = texture.slice_pitch(),
                            .data = assets_writer.write("std::byte", texture.data()),
                        });
                    }
                },
                [&](const AssetTaskProceduralCube& task) {
                    // Generate cube.
                    using namespace DirectX::DX12;
                    using Vertices = GeometricPrimitive::VertexCollection;
                    using Indices = GeometricPrimitive::IndexCollection;
                    Vertices dxtk_vertices;
                    Indices dxtk_indices;
                    GeometricPrimitive::CreateBox(
                        dxtk_vertices,
                        dxtk_indices,
                        {task.extents, task.extents, task.extents},
                        !task.inverted,
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
                    const auto mesh_name = std::format("{}_mesh", task.name);
                    update_unique_names(unique_names, mesh_name);
                    assets.push_back(AssetMesh {
                        .name = mesh_name,
                        .vertices =
                            assets_writer.write("Vertex", std::span<const AssetVertex>(vertices)),
                        .indices =
                            assets_writer.write("Index", std::span<const AssetIndex>(indices)),
                    });
                },
            },
            asset_task
        );
    }

    FB_LOG_INFO("Built {} assets ", assets.size());

    return {assets, assets_bin};
}

} // namespace fb

namespace fb {

struct ShaderTask {
    std::string_view path;
    std::string_view name;
    std::vector<std::string_view> entry_points;
};

static auto shader_tasks = std::to_array<ShaderTask>({
    {"demos/gui.hlsl", "gui", {"draw_vs", "draw_ps"}},
    {"demos/debug_draw.hlsl", "debug_draw", {"draw_vs", "draw_ps"}},
    {"demos/cards.hlsl", "cards", {"draw_vs", "draw_ps"}},
    {"demos/spd.hlsl", "spd", {"downsample_cs"}},
    {"demos/crate/crate.hlsl", "crate", {"draw_vs", "draw_ps"}},
    {"demos/tree/tree.hlsl", "tree", {"shadow_vs", "draw_vs", "draw_ps"}},
    {"demos/rain/rain.hlsl", "rain", {"sim_cs", "draw_vs", "draw_ps"}},
    {"demos/anim/anim.hlsl", "anim", {"draw_vs", "draw_ps"}},
    {
        "demos/fibers/fibers.hlsl",
        "fibers",
        {
            "sim_cs",
            "reset_cs",
            "cull_cs",
            "light_vs",
            "light_ps",
            "plane_vs",
            "plane_ps",
            "debug_vs",
            "debug_ps",
        },
    },
    {"demos/env/env.hlsl", "env", {"draw_vs", "draw_ps"}},
});

auto shader_type_from_entry_point(std::string_view entry_point) -> ShaderType {
    if (entry_point.ends_with("_vs")) {
        return ShaderType::Vertex;
    } else if (entry_point.ends_with("_ps")) {
        return ShaderType::Pixel;
    } else if (entry_point.ends_with("_cs")) {
        return ShaderType::Compute;
    } else {
        return ShaderType::Unknown;
    }
}

auto make_unique_shader_name(std::string_view name, std::string_view entry_point) {
    return std::format("{}_{}", name, entry_point);
}

auto string_replace(std::string_view str, std::string_view from, std::string_view to)
    -> std::string {
    auto temp = std::string(str);
    temp.replace(str.find(from), from.size(), to);
    return temp;
}

auto build_shaders(std::string_view buffet_dir) -> std::vector<Shader> {
    const auto compiler = ShaderCompiler();
    auto compiled_shaders = std::vector<Shader>();
    auto unique_names = std::unordered_set<std::string>();
    auto update_unique_names = [](std::unordered_set<std::string>& uniques,
                                  const std::string& name) {
        if (uniques.contains(name)) {
            FB_LOG_ERROR("Duplicate shader name: {}", name);
            FB_FATAL();
        }
        uniques.insert(name);
    };
    for (const auto& shader_task : shader_tasks) {
        const auto hlsl_path = std::format("{}/{}", buffet_dir, shader_task.path);
        const auto hlsl_file = read_whole_file(hlsl_path);

        for (const auto& entry_point : shader_task.entry_points) {
            // Ensure unique shader names.
            const auto name = make_unique_shader_name(shader_task.name, entry_point);
            update_unique_names(unique_names, name);

            // Determine shader type.
            const auto type = shader_type_from_entry_point(entry_point);
            FB_ASSERT(type != ShaderType::Unknown);

            // Compile shader.
            const auto shader = compiler.compile(name, type, entry_point, hlsl_file, false);

            // Save shader.
            compiled_shaders.push_back(shader);
        }
    }

    FB_LOG_INFO("Built {} shaders", compiled_shaders.size());

    return compiled_shaders;
}

} // namespace fb

template<>
struct std::formatter<DXGI_FORMAT>: std::formatter<char> {
    template<class FormatContext>
    auto format(DXGI_FORMAT v, FormatContext& fc) const {
        switch (v) {
            case DXGI_FORMAT_R8G8B8A8_UNORM:
                return std::format_to(fc.out(), "DXGI_FORMAT_R8G8B8A8_UNORM");
            case DXGI_FORMAT_R8G8B8A8_UNORM_SRGB:
                return std::format_to(fc.out(), "DXGI_FORMAT_R8G8B8A8_UNORM_SRGB");
            case DXGI_FORMAT_R32G32B32A32_FLOAT:
                return std::format_to(fc.out(), "DXGI_FORMAT_R32G32B32A32_FLOAT");
            default: FB_FATAL();
        }
    }
};

int main() {
    using namespace fb;

    // Console.
    attach_console();

    // Build.
    const auto buffet_dir = std::format("{}/src/buffet", FRAMEBUFFET_SOURCE_DIR);
    const auto assets_dir = std::format("{}/src/assets", FRAMEBUFFET_SOURCE_DIR);
    const auto compiled_shaders = build_shaders(buffet_dir);
    const auto compiled_assets = build_assets(assets_dir);
    const auto& [assets, assets_bin] = compiled_assets;

    //
    // baked.hpp/cpp
    //

    std::string baked_hpp = R"(#pragma once

        #include "../pch.hpp"

        namespace fb::baked {

        struct Copy {
            std::span<const std::byte> data;
        };

        struct Vertex {
            Float3 position;
            Float3 normal;
            Float2 texcoord;
            Float4 tangent;
        };

        struct SkinningVertex {
            Float3 position;
            Float3 normal;
            Float2 texcoord;
            Float4 tangent;
            Uint4 joints;
            Float4 weights;
        };

        using Index = uint32_t;

        struct Mesh {
            std::span<const Vertex> vertices;
            std::span<const Index> indices;
        };

        struct Texture {
            DXGI_FORMAT format;
            uint32_t width;
            uint32_t height;
            uint32_t channels;
            uint32_t row_pitch;
            uint32_t slice_pitch;
            std::span<const std::byte> data;
        };

        enum class CubeFace : uint32_t {
            PosX,
            NegX,
            PosY,
            NegY,
            PosZ,
            NegZ,
        };

        struct CubeTexture {
            DXGI_FORMAT format;
            uint32_t width;
            uint32_t height;
            uint32_t channels;
            uint32_t row_pitch;
            uint32_t slice_pitch;
            std::array<std::span<const float>, 6> datas;
        };

        struct AnimationChannel {
            size_t t_offset;
            size_t t_count;
            size_t r_offset;
            size_t r_count;
            size_t s_offset;
            size_t s_count;
        };

        struct AnimationMesh {
            uint32_t node_count;
            uint32_t joint_count;
            float duration;
            std::span<const SkinningVertex> skinning_vertices;
            std::span<const Index> indices;
            std::span<const uint32_t> joint_nodes;
            std::span<const Float4x4> joint_inverse_binds;
            std::span<const uint32_t> node_parents;
            std::span<const Float4x4> node_transforms;
            std::span<const AnimationChannel> node_channels;
            std::span<const float> node_channels_times_t;
            std::span<const float> node_channels_times_r;
            std::span<const float> node_channels_times_s;
            std::span<const Float3> node_channels_values_t;
            std::span<const Quaternion> node_channels_values_r;
            std::span<const Float3> node_channels_values_s;
        };

        class Assets {
        public:
            Assets();

            {{asset_decls}}

        private:
            template<typename T>
            auto transmuted_span(size_t offset, size_t element_size) const -> std::span<const T> {
                return std::span((const T*)(_data.data() + offset), element_size);
            }

            std::vector<std::byte> _data;
        };

        class Shaders {
        public:
            Shaders();

            {{shader_decls}}

        private:
            std::vector<std::byte> _data;
        };

        } // namespace fb::baked
    )";

    std::string baked_cpp = R"(#include "baked.hpp"

        namespace fb::baked {

        Assets::Assets() {
            _data = read_whole_file("fb_assets.bin");
            FB_ASSERT(_data.size() == {{assets_total_size}});
        }

        {{asset_defns}}

        Shaders::Shaders() {
            _data = read_whole_file("fb_shaders.bin");
            FB_ASSERT(_data.size() == {{shaders_total_size}});
        }

        {{shader_defns}}

        } // namespace fb::baked
    )";

    std::ostringstream assets_decls;
    std::ostringstream assets_defns;
    for (const auto& asset : assets) {
        std::visit(
            overloaded {
                [&](const AssetCopy& asset) {
                    assets_decls << std::format("auto {}() const -> Copy;", asset.name);
                    assets_defns << std::format(
                        R"(auto Assets::{}() const -> Copy {{
                            return Copy {{
                                .data = transmuted_span<{}>({}, {}),
                            }};
                        }})",
                        asset.name,
                        asset.data.type,
                        asset.data.offset,
                        asset.data.byte_size
                    );
                },
                [&](const AssetMesh& asset) {
                    assets_decls << std::format("auto {}() const -> Mesh;", asset.name);
                    assets_defns << std::format(
                        R"(auto Assets::{}() const -> Mesh {{
                            return Mesh {{
                                .vertices = transmuted_span<{}>({}, {}),
                                .indices = transmuted_span<{}>({}, {}),
                            }};
                        }})",
                        asset.name,
                        asset.vertices.type,
                        asset.vertices.offset,
                        asset.vertices.element_size,
                        asset.indices.type,
                        asset.indices.offset,
                        asset.indices.element_size
                    );
                },
                [&](const AssetTexture& asset) {
                    assets_decls << std::format("auto {}() const -> Texture;", asset.name);
                    assets_defns << std::format(
                        R"(auto Assets::{}() const -> Texture {{
                            return Texture {{
                                .format = {},
                                .width = {},
                                .height = {},
                                .channels = {},
                                .row_pitch = {},
                                .slice_pitch = {},
                                .data = transmuted_span<{}>({}, {}),
                            }};
                        }})",
                        asset.name,
                        asset.format,
                        asset.width,
                        asset.height,
                        asset.channels,
                        asset.row_pitch,
                        asset.slice_pitch,
                        asset.data.type,
                        asset.data.offset,
                        asset.data.element_size
                    );
                },
                [&](const AssetCubeTexture& asset) {
                    assets_decls << std::format("auto {}() const -> CubeTexture;", asset.name);
                    assets_defns << std::format(
                        R"(auto Assets::{}() const -> CubeTexture {{
                            return CubeTexture {{
                                .format = {},
                                .width = {},
                                .height = {},
                                .channels = {},
                                .row_pitch = {},
                                .slice_pitch = {},
                                .datas = std::to_array({{
                                    transmuted_span<{}>({}, {}),
                                    transmuted_span<{}>({}, {}),
                                    transmuted_span<{}>({}, {}),
                                    transmuted_span<{}>({}, {}),
                                    transmuted_span<{}>({}, {}),
                                    transmuted_span<{}>({}, {}),
                                }}),
                            }};
                        }})",
                        asset.name,
                        asset.format,
                        asset.width,
                        asset.height,
                        asset.channels,
                        asset.row_pitch,
                        asset.slice_pitch,
                        asset.datas[0].type,
                        asset.datas[0].offset,
                        asset.datas[0].element_size,
                        asset.datas[1].type,
                        asset.datas[1].offset,
                        asset.datas[1].element_size,
                        asset.datas[2].type,
                        asset.datas[2].offset,
                        asset.datas[2].element_size,
                        asset.datas[3].type,
                        asset.datas[3].offset,
                        asset.datas[3].element_size,
                        asset.datas[4].type,
                        asset.datas[4].offset,
                        asset.datas[4].element_size,
                        asset.datas[5].type,
                        asset.datas[5].offset,
                        asset.datas[5].element_size
                    );
                },
                [&](const AssetAnimationMesh& asset) {
                    assets_decls << std::format("auto {}() const -> AnimationMesh;", asset.name);
                    assets_defns << std::format(
                        R"(auto Assets::{}() const -> AnimationMesh {{
                            return AnimationMesh {{
                                .node_count = {},
                                .joint_count = {},
                                .duration = {}f,
                                .skinning_vertices = transmuted_span<{}>({}, {}),
                                .indices = transmuted_span<{}>({}, {}),
                                .joint_nodes = transmuted_span<{}>({}, {}),
                                .joint_inverse_binds = transmuted_span<{}>({}, {}),
                                .node_parents = transmuted_span<{}>({}, {}),
                                .node_transforms = transmuted_span<{}>({}, {}),
                                .node_channels = transmuted_span<{}>({}, {}),
                                .node_channels_times_t = transmuted_span<{}>({}, {}),
                                .node_channels_times_r = transmuted_span<{}>({}, {}),
                                .node_channels_times_s = transmuted_span<{}>({}, {}),
                                .node_channels_values_t = transmuted_span<{}>({}, {}),
                                .node_channels_values_r = transmuted_span<{}>({}, {}),
                                .node_channels_values_s = transmuted_span<{}>({}, {}),
                            }};
                        }})",
                        asset.name,
                        asset.node_count,
                        asset.joint_count,
                        asset.duration,
                        asset.skinning_vertices.type,
                        asset.skinning_vertices.offset,
                        asset.skinning_vertices.element_size,
                        asset.indices.type,
                        asset.indices.offset,
                        asset.indices.element_size,
                        asset.joint_nodes.type,
                        asset.joint_nodes.offset,
                        asset.joint_nodes.element_size,
                        asset.joint_inverse_binds.type,
                        asset.joint_inverse_binds.offset,
                        asset.joint_inverse_binds.element_size,
                        asset.node_parents.type,
                        asset.node_parents.offset,
                        asset.node_parents.element_size,
                        asset.node_transforms.type,
                        asset.node_transforms.offset,
                        asset.node_transforms.element_size,
                        asset.node_channels.type,
                        asset.node_channels.offset,
                        asset.node_channels.element_size,
                        asset.node_channels_times_t.type,
                        asset.node_channels_times_t.offset,
                        asset.node_channels_times_t.element_size,
                        asset.node_channels_times_r.type,
                        asset.node_channels_times_r.offset,
                        asset.node_channels_times_r.element_size,
                        asset.node_channels_times_s.type,
                        asset.node_channels_times_s.offset,
                        asset.node_channels_times_s.element_size,
                        asset.node_channels_values_t.type,
                        asset.node_channels_values_t.offset,
                        asset.node_channels_values_t.element_size,
                        asset.node_channels_values_r.type,
                        asset.node_channels_values_r.offset,
                        asset.node_channels_values_r.element_size,
                        asset.node_channels_values_s.type,
                        asset.node_channels_values_s.offset,
                        asset.node_channels_values_s.element_size
                    );
                },
            },
            asset
        );
        assets_defns << "\n\n";
    }

    std::string shader_decls;
    std::string shader_defns;
    std::vector<std::byte> shaders_bin;
    for (const auto& shader : compiled_shaders) {
        shader_decls += std::format("auto {}() const -> std::span<const std::byte>;", shader.name);
        shader_defns += std::format(
            R"(
                // {}
                auto Shaders::{}() const -> std::span<const std::byte> {{
                    return std::span(_data).subspan({}, {});
                }}
            )",
            shader.hash,
            shader.name,
            shaders_bin.size(),
            shader.dxil.size()
        );
        shaders_bin.insert(shaders_bin.end(), shader.dxil.begin(), shader.dxil.end());
    }

    baked_hpp = string_replace(baked_hpp, "{{asset_decls}}", assets_decls.str());
    baked_cpp = string_replace(baked_cpp, "{{asset_defns}}", assets_defns.str());
    baked_cpp =
        string_replace(baked_cpp, "{{assets_total_size}}", std::to_string(assets_bin.size()));

    baked_hpp = string_replace(baked_hpp, "{{shader_decls}}", shader_decls);
    baked_cpp = string_replace(baked_cpp, "{{shader_defns}}", shader_defns);
    baked_cpp =
        string_replace(baked_cpp, "{{shaders_total_size}}", std::to_string(shaders_bin.size()));

    const auto baked_dir = std::format("{}/baked", buffet_dir);
    std::string baked_cpp_path = std::format("{}/baked.cpp", baked_dir);
    std::string baked_hpp_path = std::format("{}/baked.hpp", baked_dir);
    write_whole_file(baked_cpp_path, std::as_bytes(std::span(baked_cpp)));
    write_whole_file(baked_hpp_path, std::as_bytes(std::span(baked_hpp)));
    const auto clangformat = std::format("{}/external/clang-format.exe", FRAMEBUFFET_SOURCE_DIR);
    std::system(std::format("{} -i {}", clangformat, baked_cpp_path).c_str());
    std::system(std::format("{} -i {}", clangformat, baked_hpp_path).c_str());

    //
    // Write output files.
    //

    write_whole_file(
        std::format("{}/fb_shaders.bin", FRAMEBUFFET_OUTPUT_DIR),
        std::as_bytes(std::span(shaders_bin))
    );

    write_whole_file(
        std::format("{}/fb_assets.bin", FRAMEBUFFET_OUTPUT_DIR),
        std::as_bytes(std::span(assets_bin))
    );

    const auto pdb_dir = std::format("{}/shaders", FRAMEBUFFET_OUTPUT_DIR);
    std::filesystem::create_directory(pdb_dir);
    for (const auto& shader : compiled_shaders) {
        write_whole_file(
            std::format("{}/{}.pdb", pdb_dir, shader.hash),
            std::as_bytes(std::span(shader.pdb))
        );
    }

    return 0;
}
