#include <common/common.hpp>
#include "shaders/shaders.hpp"
#include "formats/gltf.hpp"
#include "formats/mikktspace.hpp"

#include <stb_image_resize.h>

namespace fb {

//
// Asset tasks.
//

enum class AssetColorSpace {
    Srgb,
    Linear,
};

struct AssetTaskCopy {
    std::string_view name;
    std::string_view path;
};

struct AssetTaskTexture {
    std::string_view name;
    std::string_view path;
    DXGI_FORMAT format;
    AssetColorSpace color_space;
};

struct AssetTaskHdrTexture {
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
    AssetTaskHdrTexture,
    AssetTaskCubeTexture,
    AssetTaskGltf,
    AssetTaskProceduralCube>;

static auto asset_tasks = std::to_array<AssetTask>({
    AssetTaskCopy {"imgui_font", "fonts/Roboto-Medium.ttf"},
    AssetTaskTexture {
        "heatmap_magma",
        "heatmaps/magma.png",
        GLTF_BASE_COLOR_TEXTURE_FORMAT,
        AssetColorSpace::Srgb},
    AssetTaskTexture {
        "heatmap_viridis",
        "heatmaps/viridis.png",
        GLTF_BASE_COLOR_TEXTURE_FORMAT,
        AssetColorSpace::Srgb},
    AssetTaskGltf {"sci_fi_case", "models/sci_fi_case.glb"},
    AssetTaskGltf {"metal_plane", "models/metal_plane.glb"},
    AssetTaskGltf {"coconut_tree", "models/coconut_tree.glb"},
    AssetTaskGltf {"sand_plane", "models/sand_plane.glb"},
    AssetTaskGltf {"raccoon", "models/low-poly_racoon_run_animation.glb"},
    AssetTaskProceduralCube {"light_bounds", 2.0f, false},
    AssetTaskProceduralCube {"skybox", 2.0f, true},
    AssetTaskCubeTexture {
        "winter_evening",
        {
            "envmaps/winter_evening_1k_+X.exr",
            "envmaps/winter_evening_1k_-X.exr",
            "envmaps/winter_evening_1k_+Y.exr",
            "envmaps/winter_evening_1k_-Y.exr",
            "envmaps/winter_evening_1k_+Z.exr",
            "envmaps/winter_evening_1k_-Z.exr",
        },
    },
    AssetTaskHdrTexture {
        "farm_field",
        "envmaps/farm_field_2k.exr",
    },
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

struct AssetTextureData {
    uint32_t row_pitch;
    uint32_t slice_pitch;
    AssetSpan data;
};

inline constexpr uint32_t MAX_MIP_COUNT = 12;

struct AssetTexture {
    std::string name;

    DXGI_FORMAT format;
    uint32_t width;
    uint32_t height;
    uint32_t channel_count;
    uint32_t mip_count;
    std::array<AssetTextureData, MAX_MIP_COUNT> datas;
};

struct AssetCubeTexture {
    std::string name;

    DXGI_FORMAT format;
    uint32_t width;
    uint32_t height;
    uint32_t channel_count;
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
            .element_size = elements.size(),
            .byte_size = elements.size_bytes(),
        };
    }

private:
    std::vector<std::byte>& _data;
};

class UniqueNames {
public:
    auto unique(std::string_view name) -> std::string {
        if (_names.contains(name)) {
            FB_LOG_ERROR("Duplicate asset name: {}", name);
            FB_FATAL();
        }
        _names.insert(name);
        return std::string(name);
    }

private:
    std::unordered_set<std::string_view> _names;
};

auto compute_mipmaps_and_write(
    AssetsWriter& assets_writer,
    std::vector<Asset>& assets,
    const std::string& texture_name,
    const LdrImage& texture,
    DXGI_FORMAT texture_format,
    AssetColorSpace color_space
) -> void {
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

    // Write asset.
    assets.push_back(AssetTexture {
        .name = texture_name,
        .format = texture_format,
        .width = texture.width(),
        .height = texture.height(),
        .channel_count = texture.channel_count(),
        .mip_count = mip_count,
        .datas = texture_datas,
    });
}

auto build_assets(std::string_view assets_dir)
    -> std::tuple<std::vector<Asset>, std::vector<std::byte>> {
    auto assets = std::vector<Asset>();
    auto assets_bin = std::vector<std::byte>();
    auto assets_writer = AssetsWriter(assets_bin);
    auto names = UniqueNames();
    for (const auto& asset_task : asset_tasks) {
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
                    compute_mipmaps_and_write(
                        assets_writer,
                        assets,
                        name,
                        image,
                        task.format,
                        task.color_space
                    );
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
                        .channel_count = image.channel_count(),
                        .mip_count = 1,
                        .datas = {AssetTextureData {
                            .row_pitch = image.row_pitch(),
                            .slice_pitch = image.slice_pitch(),
                            .data = assets_writer.write("std::byte", image.data()),
                        }},
                    });
                },
                [&](const AssetTaskCubeTexture& task) {
                    // Name.
                    const auto name = names.unique(std::format("{}_cube_texture", task.name));

                    // Read face files.
                    const auto cube_face_datas = std::to_array({
                        read_whole_file(std::format("{}/{}", assets_dir, task.paths[0])),
                        read_whole_file(std::format("{}/{}", assets_dir, task.paths[1])),
                        read_whole_file(std::format("{}/{}", assets_dir, task.paths[2])),
                        read_whole_file(std::format("{}/{}", assets_dir, task.paths[3])),
                        read_whole_file(std::format("{}/{}", assets_dir, task.paths[4])),
                        read_whole_file(std::format("{}/{}", assets_dir, task.paths[5])),
                    });

                    // Load faces.
                    auto cube_faces = std::to_array({
                        HdrImage::load(cube_face_datas[0]), // PosX
                        HdrImage::load(cube_face_datas[1]), // NegX
                        HdrImage::load(cube_face_datas[2]), // PosY
                        HdrImage::load(cube_face_datas[3]), // NegY
                        HdrImage::load(cube_face_datas[4]), // PosZ
                        HdrImage::load(cube_face_datas[5]), // NegZ
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
                        datas[i] = assets_writer.write("std::byte", cube_faces[i].data());
                    }
                    assets.push_back(AssetCubeTexture {
                        .name = name,
                        .format = cube_format,
                        .width = cube_face_size.x,
                        .height = cube_face_size.y,
                        .channel_count = cube_faces[0].channel_count(),
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
                        compute_mipmaps_and_write(
                            assets_writer,
                            assets,
                            texture_name,
                            texture,
                            GLTF_BASE_COLOR_TEXTURE_FORMAT,
                            AssetColorSpace::Srgb
                        );
                    }
                    if (model.normal_texture().has_value()) {
                        const auto texture = model.normal_texture().value().get();
                        const auto texture_name =
                            names.unique(std::format("{}_normal_texture", task.name));
                        compute_mipmaps_and_write(
                            assets_writer,
                            assets,
                            texture_name,
                            texture,
                            GLTF_NORMAL_TEXTURE_FORMAT,
                            AssetColorSpace::Linear
                        );
                    }
                    if (model.metallic_roughness_texture().has_value()) {
                        const auto texture = model.metallic_roughness_texture().value().get();
                        const auto texture_name =
                            names.unique(std::format("{}_metallic_roughness_texture", task.name));
                        compute_mipmaps_and_write(
                            assets_writer,
                            assets,
                            texture_name,
                            texture,
                            GLTF_METALLIC_ROUGHNESS_TEXTURE_FORMAT,
                            AssetColorSpace::Linear
                        );
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
    {
        "demos/env/env.hlsl",
        "env",
        {
            "cfr_cs",
            "lut_cs",
            "background_vs",
            "background_ps",
            "screen_vs",
            "screen_ps",
        },
    },
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

        inline constexpr uint32_t MAX_MIP_COUNT = {{max_mip_count}};

        struct TextureData {
            uint32_t row_pitch;
            uint32_t slice_pitch;
            std::span<const std::byte> data;
        };

        struct Texture {
            DXGI_FORMAT format;
            uint32_t width;
            uint32_t height;
            uint32_t channel_count;
            uint32_t mip_count;
            std::array<TextureData, MAX_MIP_COUNT> datas;
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
            uint32_t channel_count;
            uint32_t row_pitch;
            uint32_t slice_pitch;
            std::array<std::span<const std::byte>, 6> datas;
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

    const auto format_asset_span = [&assets_bin](AssetSpan span) -> std::string {
        const auto bytes = std::span(assets_bin.data() + span.offset, span.byte_size);
        const auto hash = hash128(bytes);
        return std::format(
            "// hash: {}\n transmuted_span<{}>({}, {})",
            hash,
            span.type,
            span.offset,
            span.element_size
        );
    };
    const auto format_named_asset_span =
        [&assets_bin](std::string_view name, AssetSpan span) -> std::string {
        const auto bytes = std::span(assets_bin.data() + span.offset, span.byte_size);
        const auto hash = hash128(bytes);
        return std::format(
            "// hash: {}\n .{} = transmuted_span<{}>({}, {})",
            hash,
            name,
            span.type,
            span.offset,
            span.element_size
        );
    };

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
                                {},
                            }};
                        }})",
                        asset.name,
                        format_named_asset_span("data"sv, asset.data)
                    );
                },
                [&](const AssetMesh& asset) {
                    assets_decls << std::format("auto {}() const -> Mesh;", asset.name);
                    assets_defns << std::format(
                        R"(auto Assets::{}() const -> Mesh {{
                            return Mesh {{
                                {},
                                {},
                            }};
                        }})",
                        asset.name,
                        format_named_asset_span("vertices"sv, asset.vertices),
                        format_named_asset_span("indices"sv, asset.indices)
                    );
                },
                [&](const AssetTexture& asset) {
                    assets_decls << std::format("auto {}() const -> Texture;", asset.name);

                    std::ostringstream texture_datas;
                    uint32_t texture_width = asset.width;
                    uint32_t texture_height = asset.height;
                    for (uint32_t i = 0; i < asset.mip_count; ++i) {
                        texture_datas << std::format(
                            R"(TextureData {{
                                // mip_level: {}
                                // width: {}
                                // height: {}
                                .row_pitch = {},
                                .slice_pitch = {},
                                {},
                            }})",
                            i,
                            texture_width,
                            texture_height,
                            asset.datas[i].row_pitch,
                            asset.datas[i].slice_pitch,
                            format_named_asset_span("data"sv, asset.datas[i].data)
                        );
                        texture_width = std::max(1u, texture_width / 2);
                        texture_height = std::max(1u, texture_height / 2);
                        if (i != asset.mip_count - 1) {
                            texture_datas << ",\n";
                        }
                    }

                    assets_defns << std::format(
                        R"(auto Assets::{}() const -> Texture {{
                            return Texture {{
                                .format = {},
                                .width = {},
                                .height = {},
                                .channel_count = {},
                                .mip_count = {},
                                .datas = {{{}}},
                            }};
                        }})",
                        asset.name,
                        asset.format,
                        asset.width,
                        asset.height,
                        asset.channel_count,
                        asset.mip_count,
                        texture_datas.str()
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
                                .channel_count = {},
                                .row_pitch = {},
                                .slice_pitch = {},
                                .datas = std::to_array({{
                                    {},
                                    {},
                                    {},
                                    {},
                                    {},
                                    {},
                                }}),
                            }};
                        }})",
                        asset.name,
                        asset.format,
                        asset.width,
                        asset.height,
                        asset.channel_count,
                        asset.row_pitch,
                        asset.slice_pitch,
                        format_asset_span(asset.datas[0]),
                        format_asset_span(asset.datas[1]),
                        format_asset_span(asset.datas[2]),
                        format_asset_span(asset.datas[3]),
                        format_asset_span(asset.datas[4]),
                        format_asset_span(asset.datas[5])
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
                                {},
                                {},
                                {},
                                {},
                                {},
                                {},
                                {},
                                {},
                                {},
                                {},
                                {},
                                {},
                                {},
                            }};
                        }})",
                        asset.name,
                        asset.node_count,
                        asset.joint_count,
                        asset.duration,
                        format_named_asset_span("skinning_vertices"sv, asset.skinning_vertices),
                        format_named_asset_span("indices"sv, asset.indices),
                        format_named_asset_span("joint_nodes"sv, asset.joint_nodes),
                        format_named_asset_span("joint_inverse_binds"sv, asset.joint_inverse_binds),
                        format_named_asset_span("node_parents"sv, asset.node_parents),
                        format_named_asset_span("node_transforms"sv, asset.node_transforms),
                        format_named_asset_span("node_channels"sv, asset.node_channels),
                        format_named_asset_span(
                            "node_channels_times_t"sv,
                            asset.node_channels_times_t
                        ),
                        format_named_asset_span(
                            "node_channels_times_r"sv,
                            asset.node_channels_times_r
                        ),
                        format_named_asset_span(
                            "node_channels_times_s"sv,
                            asset.node_channels_times_s
                        ),
                        format_named_asset_span(
                            "node_channels_values_t"sv,
                            asset.node_channels_values_t
                        ),
                        format_named_asset_span(
                            "node_channels_values_r"sv,
                            asset.node_channels_values_r
                        ),
                        format_named_asset_span(
                            "node_channels_values_s"sv,
                            asset.node_channels_values_s
                        )
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
                // shader_hash: {}
                {}
                auto Shaders::{}() const -> std::span<const std::byte> {{
                    return std::span(_data).subspan({}, {});
                }}
            )",
            shader.hash,
            shader.counters.to_comment_string(),
            shader.name,
            shaders_bin.size(),
            shader.dxil.size()
        );
        shaders_bin.insert(shaders_bin.end(), shader.dxil.begin(), shader.dxil.end());
    }

    baked_hpp = string_replace(baked_hpp, "{{max_mip_count}}", std::format("{}", MAX_MIP_COUNT));
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

    std::filesystem::create_directories(FRAMEBUFFET_OUTPUT_DIR);

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
