#include "outputs.hpp"
#include "templates/baked_types_hpp.hpp"
#include "templates/baked_hpp.hpp"
#include "templates/baked_cpp.hpp"

#include <sstream>

template<>
struct std::formatter<DXGI_FORMAT>: std::formatter<char> {
    template<class FormatContext>
    auto format(DXGI_FORMAT v, FormatContext& fc) const {
        switch (v) {
            case DXGI_FORMAT_R8G8B8A8_UNORM:
                return std::format_to(fc.out(), "DXGI_FORMAT_R8G8B8A8_UNORM");
            case DXGI_FORMAT_R8G8B8A8_UNORM_SRGB:
                return std::format_to(fc.out(), "DXGI_FORMAT_R8G8B8A8_UNORM_SRGB");
            case DXGI_FORMAT_R16G16_FLOAT:
                return std::format_to(fc.out(), "DXGI_FORMAT_R16G16_FLOAT");
            case DXGI_FORMAT_R16G16B16A16_FLOAT:
                return std::format_to(fc.out(), "DXGI_FORMAT_R16G16B16A16_FLOAT");
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
    const auto baked_app_dir = std::format("{}/{}", baked_dir, app_name);
    const auto assets_dir = std::format("{}/src/assets", FB_SOURCE_DIR);
    const auto baked_types_hpp_path = std::format("{}/baked_types.hpp", baked_dir);
    const auto baked_hpp_path = std::format("{}/baked.hpp", baked_app_dir);
    const auto baked_cpp_path = std::format("{}/baked.cpp", baked_app_dir);
    const auto clangformat = std::format("{}/external/clang-format.exe", FB_SOURCE_DIR);

    // Bake.
    const auto compiled_shaders = bake_shaders(source_dir, app_shader_tasks);
    const auto [assets, assets_bin] = bake_assets(assets_dir, app_asset_tasks);

    // Generate.
    const auto format_asset_span = [&assets_bin](AssetSpan span) -> std::string {
        const auto bytes = std::span(assets_bin.data() + span.offset, span.byte_count);
        const auto hash = hash128(bytes);
        return std::format(
            "// hash: {}\n transmuted_span<{}>({}, {})",
            hash,
            span.type,
            span.offset,
            span.element_count
        );
    };
    const auto format_named_asset_span =
        [&assets_bin](std::string_view name, AssetSpan span) -> std::string {
        const auto bytes = std::span(assets_bin.data() + span.offset, span.byte_count);
        const auto hash = hash128(bytes);
        return std::format(
            "// hash: {}\n .{} = transmuted_span<{}>({}, {})",
            hash,
            name,
            span.type,
            span.offset,
            span.element_count
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
                        asset.vertices.element_count,
                        asset.indices.element_count / 3,
                        format_named_asset_span("vertices"sv, asset.vertices),
                        format_named_asset_span("indices"sv, asset.indices)
                    );
                },
                [&](const AssetTexture& asset) {
                    assets_decls << std::format("auto {}() const -> Texture;", asset.name);

                    std::ostringstream texture_datas;
                    for (uint mip = 0; mip < asset.mip_count; ++mip) {
                        const auto mip_width = std::max(1u, asset.width >> mip);
                        const auto mip_height = std::max(1u, asset.height >> mip);
                        const auto span = asset.datas[mip].data;
                        const auto bytes =
                            std::span(assets_bin.data() + span.offset, span.byte_count);
                        const auto hash = hash128(bytes);
                        if (mip > 0) {
                            texture_datas << "\n";
                        }
                        texture_datas << std::format(
                            "    datas[{:2}] = texture_data({:4}, {:7}, {:9}, {:7}); // hash: {}, width: {}, height: {}",
                            mip,
                            asset.datas[mip].row_pitch,
                            asset.datas[mip].slice_pitch,
                            asset.datas[mip].data.offset,
                            asset.datas[mip].data.element_count,
                            hash,
                            mip_width,
                            mip_height
                        );
                    }

                    assets_defns << std::format(
                        R"(auto Assets::{}() const -> Texture {{
                            decltype(Texture::datas) datas = {{}};
                            // clang-format off
{}
                            // clang-format on
                            return Texture {{
                                .format = {},
                                .width = {},
                                .height = {},
                                .mip_count = {},
                                .datas = datas,
                            }};
                        }})",
                        asset.name,
                        texture_datas.str(),
                        asset.format,
                        asset.width,
                        asset.height,
                        asset.mip_count
                    );
                },
                [&](const AssetCubeTexture& asset) {
                    assets_decls << std::format("auto {}() const -> CubeTexture;", asset.name);

                    std::ostringstream texture_datas;
                    for (uint slice = 0; slice < 6; slice++) {
                        const auto& slice_datas = asset.datas[slice];
                        for (uint mip = 0; mip < asset.mip_count; mip++) {
                            const auto mip_width = std::max(1u, asset.width >> mip);
                            const auto mip_height = std::max(1u, asset.height >> mip);
                            const auto span = slice_datas[mip].data;
                            const auto bytes =
                                std::span(assets_bin.data() + span.offset, span.byte_count);
                            const auto hash = hash128(bytes);
                            if (slice > 0 || mip > 0) {
                                texture_datas << "\n";
                            }
                            texture_datas << std::format(
                                R"(    datas[{}][{:2}] = texture_data({:4}, {:7}, {:9}, {:7}); // hash: {}, width: {}, height: {})",
                                slice,
                                mip,
                                slice_datas[mip].row_pitch,
                                slice_datas[mip].slice_pitch,
                                slice_datas[mip].data.offset,
                                slice_datas[mip].data.element_count,
                                hash,
                                mip_width,
                                mip_height
                            );
                        }
                    }

                    assets_defns << std::format(
                        R"(auto Assets::{}() const -> CubeTexture {{
                            decltype(CubeTexture::datas) datas = {{}};
                            // clang-format off
{}
                            // clang-format on
                            return CubeTexture {{
                                .format = {},
                                .width = {},
                                .height = {},
                                .mip_count = {},
                                .datas = datas,
                            }};
                        }})",
                        asset.name,
                        texture_datas.str(),
                        asset.format,
                        asset.width,
                        asset.height,
                        asset.mip_count
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
    baked_cpp = str_replace(baked_cpp, "{{assets_byte_count}}", std::to_string(assets_bin.size()));
    baked_cpp = str_replace(baked_cpp, "{{shader_defns}}", shader_defns.str());
    baked_cpp = str_replace(baked_cpp, "{{shaders_byte_count}}", std::to_string(shaders_bin.size()));
    // clang-format on

    // Write source files.
    create_directories(baked_app_dir);
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

        create_directories(output_dir);
        create_directory(shaders_dir);

        write_whole_file(assets_bin_file, assets_bin);
        write_whole_file(shaders_bin_file, shaders_bin);
        for (const auto& shader : compiled_shaders) {
            const auto pdb_file_path = std::format("{}/{}.pdb", shaders_dir, shader.hash);
            write_whole_file(pdb_file_path, shader.pdb);
        }
    }
}

} // namespace fb
