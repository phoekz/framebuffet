#include "outputs.hpp"
#include "templates/baked_types_hpp.hpp"
#include "templates/baked_hpp.hpp"
#include "templates/baked_cpp.hpp"

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

namespace fb {

auto bake_app_datas(
    std::span<const std::string_view> output_dirs,
    std::string_view app_name,
    std::span<const AssetTask> app_asset_tasks,
    std::span<const ShaderTask> app_shader_tasks
) -> void {
    // Log.
    FB_LOG_INFO("Baking app datas: {}", app_name);

    // Paths.
    const auto source_dir = std::format("{}/src", FB_SOURCE_DIR);
    const auto baked_dir = std::format("{}/src/baked", FB_SOURCE_DIR);
    const auto assets_dir = std::format("{}/src/assets", FB_SOURCE_DIR);
    const auto baked_types_hpp_path = std::format("{}/baked_types.hpp", baked_dir);
    const auto baked_hpp_path = std::format("{}/{}/baked.hpp", baked_dir, app_name);
    const auto baked_cpp_path = std::format("{}/{}/baked.cpp", baked_dir, app_name);
    const auto clangformat = std::format("{}/external/clang-format.exe", FB_SOURCE_DIR);

    // Bake.
    const auto compiled_shaders = bake_shaders(source_dir, app_shader_tasks);
    const auto [assets, assets_bin] = bake_assets(assets_dir, app_asset_tasks);

    // Generate.
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
                            // vertex_count: {}
                            // face_count: {}
                            return Mesh {{
                                {},
                                {},
                            }};
                        }})",
                        asset.name,
                        asset.vertices.element_size,
                        asset.indices.element_size / 3,
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
                            }},)",
                            i,
                            texture_width,
                            texture_height,
                            asset.datas[i].row_pitch,
                            asset.datas[i].slice_pitch,
                            format_named_asset_span("data"sv, asset.datas[i].data)
                        );
                        texture_width = std::max(1u, texture_width / 2);
                        texture_height = std::max(1u, texture_height / 2);
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

    std::ostringstream shader_decls;
    std::ostringstream shader_defns;
    std::vector<std::byte> shaders_bin;
    for (const auto& shader : compiled_shaders) {
        shader_decls << std::format("auto {}() const -> std::span<const std::byte>;", shader.name);
        shader_defns << std::format(
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

    // Hash.
    const auto assets_bin_hash = hash128(assets_bin);
    const auto shaders_bin_hash = hash128(shaders_bin);

    // clang-format off
    auto baked_types_hpp = std::string(BAKED_TYPES_HPP);
    auto baked_hpp = std::string(BAKED_HPP);
    auto baked_cpp = std::string(BAKED_CPP);
    baked_types_hpp = str_replace(baked_types_hpp, "{{max_mip_count}}", std::format("{}", MAX_MIP_COUNT));
    baked_hpp = str_replace(baked_hpp, "{{app_name}}", app_name);
    baked_hpp = str_replace(baked_hpp, "{{asset_decls}}", assets_decls.str());
    baked_hpp = str_replace(baked_hpp, "{{shader_decls}}", shader_decls.str());
    baked_cpp = str_replace(baked_cpp, "{{app_name}}", app_name);
    baked_cpp = str_replace(baked_cpp, "{{assets_bin_hash}}", std::format("{}", assets_bin_hash));
    baked_cpp = str_replace(baked_cpp, "{{shaders_bin_hash}}", std::format("{}", shaders_bin_hash));
    baked_cpp = str_replace(baked_cpp, "{{asset_defns}}", assets_defns.str());
    baked_cpp = str_replace(baked_cpp, "{{assets_total_size}}", std::to_string(assets_bin.size()));
    baked_cpp = str_replace(baked_cpp, "{{shader_defns}}", shader_defns.str());
    baked_cpp = str_replace(baked_cpp, "{{shaders_total_size}}", std::to_string(shaders_bin.size()));
    // clang-format on

    // Write source files.
    write_whole_file(baked_types_hpp_path, std::as_bytes(std::span(baked_types_hpp)));
    write_whole_file(baked_hpp_path, std::as_bytes(std::span(baked_hpp)));
    write_whole_file(baked_cpp_path, std::as_bytes(std::span(baked_cpp)));

    // Format source files.
    const auto clangformat_cmd = std::format(
        "{} -i {} -i {} -i {}",
        clangformat,
        baked_types_hpp_path,
        baked_hpp_path,
        baked_cpp_path
    );
    std::system(clangformat_cmd.c_str());

    // Write binary files.
    for (const auto& output_dir : output_dirs) {
        const auto assets_bin_file = std::format("{}/fb_{}_assets.bin", output_dir, app_name);
        const auto shaders_dir = std::format("{}/shaders", output_dir);
        const auto shaders_bin_file = std::format("{}/fb_{}_shaders.bin", output_dir, app_name);

        std::filesystem::create_directories(output_dir);
        std::filesystem::create_directory(shaders_dir);

        write_whole_file(assets_bin_file, assets_bin);
        write_whole_file(shaders_bin_file, shaders_bin);
        for (const auto& shader : compiled_shaders) {
            const auto pdb_file_path = std::format("{}/{}.pdb", shaders_dir, shader.hash);
            write_whole_file(pdb_file_path, shader.pdb);
        }
    }
}

} // namespace fb
