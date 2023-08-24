#include "baked.hpp"

namespace fb::baked {

Assets::Assets() {
    _data = read_whole_file("fb_assets.bin");
    FB_ASSERT(_data.size() == 127530132);
}

auto Assets::imgui_font() const -> Copy {
    return Copy {
        // 799fc360204416196536a93c9eff68ae
        .data = transmuted_span<std::byte>(0, 162588),
    };
}

auto Assets::heatmap_magma_texture() const -> Texture {
    return Texture {
        .format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB,
        .width = 256,
        .height = 1,
        .channels = 4,
        .row_pitch = 1024,
        .slice_pitch = 1024,
        // 33e2d2ee5bc875a573dd90c6f664a91c
        .data = transmuted_span<std::byte>(162588, 1024),
    };
}

auto Assets::heatmap_viridis_texture() const -> Texture {
    return Texture {
        .format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB,
        .width = 256,
        .height = 1,
        .channels = 4,
        .row_pitch = 1024,
        .slice_pitch = 1024,
        // 68bceaeffeb954c3304af1cdb4a1e5fc
        .data = transmuted_span<std::byte>(163612, 1024),
    };
}

auto Assets::sci_fi_case_mesh() const -> Mesh {
    return Mesh {
        // 46dfa41b2805d9f78f0058483fdae3cc
        .vertices = transmuted_span<Vertex>(164636, 2025),
        // f00fba0022e45d0bae44c3c65770372c
        .indices = transmuted_span<Index>(261836, 5301),
    };
}

auto Assets::sci_fi_case_base_color_texture() const -> Texture {
    return Texture {
        .format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB,
        .width = 1024,
        .height = 1024,
        .channels = 4,
        .row_pitch = 4096,
        .slice_pitch = 4194304,
        // 1c0cad906e975f6b2b0f9d24de7c4926
        .data = transmuted_span<std::byte>(283040, 4194304),
    };
}

auto Assets::sci_fi_case_normal_texture() const -> Texture {
    return Texture {
        .format = DXGI_FORMAT_R8G8B8A8_UNORM,
        .width = 1024,
        .height = 1024,
        .channels = 4,
        .row_pitch = 4096,
        .slice_pitch = 4194304,
        // d5c691d45d7f61290eb8b12ed954e8f1
        .data = transmuted_span<std::byte>(4477344, 4194304),
    };
}

auto Assets::sci_fi_case_metallic_roughness_texture() const -> Texture {
    return Texture {
        .format = DXGI_FORMAT_R8G8B8A8_UNORM,
        .width = 1024,
        .height = 1024,
        .channels = 4,
        .row_pitch = 4096,
        .slice_pitch = 4194304,
        // ed04eb3dfe14725c0ad06ee90e903d7b
        .data = transmuted_span<std::byte>(8671648, 4194304),
    };
}

auto Assets::metal_plane_mesh() const -> Mesh {
    return Mesh {
        // 3a00cc1c02935599829c5692e6b36bc1
        .vertices = transmuted_span<Vertex>(12865952, 4),
        // d0af1ef28fef810ebe92063f244f24d2
        .indices = transmuted_span<Index>(12866144, 6),
    };
}

auto Assets::metal_plane_base_color_texture() const -> Texture {
    return Texture {
        .format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB,
        .width = 1024,
        .height = 1024,
        .channels = 4,
        .row_pitch = 4096,
        .slice_pitch = 4194304,
        // b01e0981487f9386f14d7e555623c2ba
        .data = transmuted_span<std::byte>(12866168, 4194304),
    };
}

auto Assets::metal_plane_normal_texture() const -> Texture {
    return Texture {
        .format = DXGI_FORMAT_R8G8B8A8_UNORM,
        .width = 1024,
        .height = 1024,
        .channels = 4,
        .row_pitch = 4096,
        .slice_pitch = 4194304,
        // 2c3220bc625827e8a3a07df006fd2577
        .data = transmuted_span<std::byte>(17060472, 4194304),
    };
}

auto Assets::metal_plane_metallic_roughness_texture() const -> Texture {
    return Texture {
        .format = DXGI_FORMAT_R8G8B8A8_UNORM,
        .width = 1024,
        .height = 1024,
        .channels = 4,
        .row_pitch = 4096,
        .slice_pitch = 4194304,
        // a0a8987101c3763d88cded178f004edc
        .data = transmuted_span<std::byte>(21254776, 4194304),
    };
}

auto Assets::coconut_tree_mesh() const -> Mesh {
    return Mesh {
        // 1fa0c357d6258a02f56e143e69f9a1a3
        .vertices = transmuted_span<Vertex>(25449080, 725),
        // 1baf1b518291c89e5c6e721974e09c51
        .indices = transmuted_span<Index>(25483880, 2034),
    };
}

auto Assets::coconut_tree_base_color_texture() const -> Texture {
    return Texture {
        .format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB,
        .width = 512,
        .height = 512,
        .channels = 4,
        .row_pitch = 2048,
        .slice_pitch = 1048576,
        // 61b50bca3a05bbee4e3297d007f5fc25
        .data = transmuted_span<std::byte>(25492016, 1048576),
    };
}

auto Assets::sand_plane_mesh() const -> Mesh {
    return Mesh {
        // dcacea845fb251ef7fac317ebd188929
        .vertices = transmuted_span<Vertex>(26540592, 4),
        // d0af1ef28fef810ebe92063f244f24d2
        .indices = transmuted_span<Index>(26540784, 6),
    };
}

auto Assets::sand_plane_base_color_texture() const -> Texture {
    return Texture {
        .format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB,
        .width = 64,
        .height = 64,
        .channels = 4,
        .row_pitch = 256,
        .slice_pitch = 16384,
        // cd9c6abea5eb3cc9b5846ab373f9d5d8
        .data = transmuted_span<std::byte>(26540808, 16384),
    };
}

auto Assets::raccoon_animation_mesh() const -> AnimationMesh {
    return AnimationMesh {
        .node_count = 72,
        .joint_count = 63,
        .duration = 0.7916667f,
        // e65e9d279ecf4a2eac69f51658a72576
        .skinning_vertices = transmuted_span<SkinningVertex>(26557192, 2430),
        // 8968eb8789a4261c4f435a57217ab6fa
        .indices = transmuted_span<Index>(26751592, 3102),
        // d730e286c5631c1fdfa6f0f7191ddd3b
        .joint_nodes = transmuted_span<uint32_t>(26764000, 63),
        // ee0ff01354a210fae3930c7992929c24
        .joint_inverse_binds = transmuted_span<Float4x4>(26764252, 63),
        // e95d90faa9a34f6172c2c7b88a91046c
        .node_parents = transmuted_span<uint32_t>(26768284, 72),
        // a3ce072a99fe8139419ff55378991f51
        .node_transforms = transmuted_span<Float4x4>(26768572, 72),
        // 2ba2f2a43abbd5968d77adad334f5682
        .node_channels = transmuted_span<AnimationChannel>(26773180, 72),
        // 3b153cfb6ae1bff95aa6dce90c05c435
        .node_channels_times_t = transmuted_span<float>(26776636, 2256),
        // c2293a9e0414fc79c6d7cd00d4c1ba87
        .node_channels_times_r = transmuted_span<float>(26785660, 2274),
        // 4d32d6eb8afd6df010a01af960891484
        .node_channels_times_s = transmuted_span<float>(26794756, 369),
        // 1defd6ee1077f6a108951a438abf4c61
        .node_channels_values_t = transmuted_span<Float3>(26796232, 2256),
        // f00545a5efeaef5e3e8a97a62d2ae87a
        .node_channels_values_r = transmuted_span<Quaternion>(26823304, 2274),
        // f56ee25392a3504e5a600f0f847638e4
        .node_channels_values_s = transmuted_span<Float3>(26859688, 369),
    };
}

auto Assets::raccoon_base_color_texture() const -> Texture {
    return Texture {
        .format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB,
        .width = 4,
        .height = 4,
        .channels = 4,
        .row_pitch = 16,
        .slice_pitch = 64,
        // f46411895f74b52626f359cce1525b8e
        .data = transmuted_span<std::byte>(26864116, 64),
    };
}

auto Assets::raccoon_metallic_roughness_texture() const -> Texture {
    return Texture {
        .format = DXGI_FORMAT_R8G8B8A8_UNORM,
        .width = 4,
        .height = 4,
        .channels = 4,
        .row_pitch = 16,
        .slice_pitch = 64,
        // 22771420d888fb100e7450610145f45d
        .data = transmuted_span<std::byte>(26864180, 64),
    };
}

auto Assets::light_bounds_mesh() const -> Mesh {
    return Mesh {
        // df492a0ab1c96806e2e0d1b0cf89141e
        .vertices = transmuted_span<Vertex>(26864244, 24),
        // 0bbe6ec9d4b61b792981857b935d2c96
        .indices = transmuted_span<Index>(26865396, 36),
    };
}

auto Assets::skybox_mesh() const -> Mesh {
    return Mesh {
        // b0d3d0c5a54476148eb9eb0df8b84d8c
        .vertices = transmuted_span<Vertex>(26865540, 24),
        // 616e076015f03b0288fd27d32b7bf256
        .indices = transmuted_span<Index>(26866692, 36),
    };
}

auto Assets::winter_evening_cube_texture() const -> CubeTexture {
    return CubeTexture {
        .format = DXGI_FORMAT_R32G32B32A32_FLOAT,
        .width = 1024,
        .height = 1024,
        .channels = 4,
        .row_pitch = 16384,
        .slice_pitch = 16777216,
        .datas = std::to_array({
            // 918af87792513fcb47721c6bb751b8ab
            transmuted_span<float>(26866836, 4194304),
            // ea87830a0e05072d855f1850fd3f22b1
            transmuted_span<float>(43644052, 4194304),
            // da32c3251997b5da09b8f00ed97cfe1d
            transmuted_span<float>(60421268, 4194304),
            // 8181516c3622f6119a276a7e317fe2d5
            transmuted_span<float>(77198484, 4194304),
            // 38d65d00389eb7511870cb5439ea27c0
            transmuted_span<float>(93975700, 4194304),
            // 49f7ddd4345a3fe231d2f04a4d7074ef
            transmuted_span<float>(110752916, 4194304),
        }),
    };
}

Shaders::Shaders() {
    _data = read_whole_file("fb_shaders.bin");
    FB_ASSERT(_data.size() == 147356);
}

// shader_hash: a2b037e929fa47e82668128ef69b5d3c
// constant_buffers: 1
// bound_resources: 1
// input_parameters: 2
// output_parameters: 3
// instruction_count: 86
// float_instruction_count: 31
// int_instruction_count: 1
// texture_load_instructions: 3
auto Shaders::gui_draw_vs() const -> std::span<const std::byte> {
    return std::span(_data).subspan(0, 5352);
}

// shader_hash: 78391642d962dafe09fcb897ff0baef4
// constant_buffers: 1
// bound_resources: 1
// input_parameters: 3
// output_parameters: 1
// instruction_count: 28
// float_instruction_count: 4
// texture_normal_instructions: 1
auto Shaders::gui_draw_ps() const -> std::span<const std::byte> {
    return std::span(_data).subspan(5352, 4416);
}

// shader_hash: 90b108a0f53edc3edf0fdbdf347b5003
// constant_buffers: 1
// bound_resources: 1
// input_parameters: 2
// output_parameters: 2
// instruction_count: 74
// float_instruction_count: 16
// texture_load_instructions: 2
auto Shaders::debug_draw_draw_vs() const -> std::span<const std::byte> {
    return std::span(_data).subspan(9768, 5120);
}

// shader_hash: 28a5f694ad5b0a73913b0a924d402ba0
// input_parameters: 2
// output_parameters: 1
// instruction_count: 9
auto Shaders::debug_draw_draw_ps() const -> std::span<const std::byte> {
    return std::span(_data).subspan(14888, 3056);
}

// shader_hash: 5792fbf56668c072f9917d0d52e90c6d
// constant_buffers: 1
// bound_resources: 1
// input_parameters: 2
// output_parameters: 2
// instruction_count: 64
// float_instruction_count: 12
// texture_load_instructions: 4
auto Shaders::cards_draw_vs() const -> std::span<const std::byte> {
    return std::span(_data).subspan(17944, 4860);
}

// shader_hash: 0277dbed47bb3692b278d7e759af728f
// constant_buffers: 1
// bound_resources: 1
// input_parameters: 2
// output_parameters: 1
// instruction_count: 20
// texture_normal_instructions: 1
auto Shaders::cards_draw_ps() const -> std::span<const std::byte> {
    return std::span(_data).subspan(22804, 4220);
}

// shader_hash: 6529e616c34b9ecea94c7de26fa74cbd
// constant_buffers: 1
// bound_resources: 1
// instruction_count: 1478
// temp_array_count: 2048
// dynamic_flow_control_count: 30
// array_instruction_count: 76
// float_instruction_count: 449
// int_instruction_count: 347
// uint_instruction_count: 78
// texture_normal_instructions: 4
// texture_load_instructions: 16
// barrier_instructions: 9
// interlocked_instructions: 1
// texture_store_instructions: 22
auto Shaders::spd_downsample_cs() const -> std::span<const std::byte> {
    return std::span(_data).subspan(27024, 13864);
}

// shader_hash: 47aeb2be7e36a430ebf09731c029cbf2
// constant_buffers: 1
// bound_resources: 1
// input_parameters: 2
// output_parameters: 5
// instruction_count: 91
// float_instruction_count: 20
// texture_load_instructions: 4
auto Shaders::crate_draw_vs() const -> std::span<const std::byte> {
    return std::span(_data).subspan(40888, 5524);
}

// shader_hash: cf5b250f80acd40de396510b0e2fe1e5
// constant_buffers: 1
// bound_resources: 1
// input_parameters: 5
// output_parameters: 1
// instruction_count: 133
// float_instruction_count: 58
// texture_normal_instructions: 3
auto Shaders::crate_draw_ps() const -> std::span<const std::byte> {
    return std::span(_data).subspan(46412, 6032);
}

// shader_hash: bb318ca5796644e7a6c68671f7011d41
// constant_buffers: 1
// bound_resources: 1
// input_parameters: 2
// output_parameters: 1
// instruction_count: 55
// float_instruction_count: 8
// texture_load_instructions: 1
auto Shaders::tree_shadow_vs() const -> std::span<const std::byte> {
    return std::span(_data).subspan(52444, 4652);
}

// shader_hash: 4f92468768d022e91d050a21946f749f
// constant_buffers: 1
// bound_resources: 1
// input_parameters: 2
// output_parameters: 4
// instruction_count: 107
// float_instruction_count: 16
// texture_load_instructions: 3
auto Shaders::tree_draw_vs() const -> std::span<const std::byte> {
    return std::span(_data).subspan(57096, 5456);
}

// shader_hash: d8fc468d518d682e1b21e1879a32d5e3
// constant_buffers: 1
// bound_resources: 1
// input_parameters: 4
// output_parameters: 1
// instruction_count: 58
// float_instruction_count: 17
// texture_normal_instructions: 1
// texture_comp_instructions: 1
auto Shaders::tree_draw_ps() const -> std::span<const std::byte> {
    return std::span(_data).subspan(62552, 5216);
}

// shader_hash: d7bde9ff419de59f4788f766651525ae
// constant_buffers: 1
// bound_resources: 1
// instruction_count: 23
// float_instruction_count: 3
// texture_load_instructions: 1
// texture_store_instructions: 1
auto Shaders::rain_sim_cs() const -> std::span<const std::byte> {
    return std::span(_data).subspan(67768, 3748);
}

// shader_hash: 0820cea972058c552f588fd8ea7b324e
// constant_buffers: 1
// bound_resources: 1
// input_parameters: 2
// output_parameters: 2
// instruction_count: 92
// float_instruction_count: 14
// texture_load_instructions: 3
auto Shaders::rain_draw_vs() const -> std::span<const std::byte> {
    return std::span(_data).subspan(71516, 5064);
}

// shader_hash: 785867d3fd40ec288756abc5b7fa1a8a
// input_parameters: 2
// output_parameters: 1
// instruction_count: 20
// float_instruction_count: 13
auto Shaders::rain_draw_ps() const -> std::span<const std::byte> {
    return std::span(_data).subspan(76580, 3228);
}

// shader_hash: 375a5653c5bb68d44e67412aa73d0ae3
// constant_buffers: 1
// bound_resources: 1
// input_parameters: 2
// output_parameters: 4
// instruction_count: 636
// float_instruction_count: 188
// texture_load_instructions: 38
auto Shaders::anim_draw_vs() const -> std::span<const std::byte> {
    return std::span(_data).subspan(79808, 10188);
}

// shader_hash: 3451eb25b73e7f9e7038c3afaf0e67a6
// constant_buffers: 1
// bound_resources: 1
// input_parameters: 4
// output_parameters: 1
// instruction_count: 29
// float_instruction_count: 7
// texture_normal_instructions: 1
auto Shaders::anim_draw_ps() const -> std::span<const std::byte> {
    return std::span(_data).subspan(89996, 4776);
}

// shader_hash: 38367bd0b1752e4ca0df66b75c2c3e42
// constant_buffers: 1
// bound_resources: 1
// instruction_count: 32
// float_instruction_count: 4
// texture_load_instructions: 3
// texture_store_instructions: 3
auto Shaders::fibers_sim_cs() const -> std::span<const std::byte> {
    return std::span(_data).subspan(94772, 4004);
}

// shader_hash: 27a8754335d1b1087f852a95afe6cf98
// constant_buffers: 1
// bound_resources: 1
// instruction_count: 8
// texture_store_instructions: 1
auto Shaders::fibers_reset_cs() const -> std::span<const std::byte> {
    return std::span(_data).subspan(98776, 3424);
}

// shader_hash: 6948df21d6f52241ffd77c4cc5ae1817
// constant_buffers: 1
// bound_resources: 1
// instruction_count: 322
// temp_array_count: 192
// dynamic_flow_control_count: 9
// array_instruction_count: 22
// float_instruction_count: 124
// int_instruction_count: 19
// uint_instruction_count: 5
// texture_load_instructions: 1
// barrier_instructions: 3
// interlocked_instructions: 2
// texture_store_instructions: 3
auto Shaders::fibers_cull_cs() const -> std::span<const std::byte> {
    return std::span(_data).subspan(102200, 7328);
}

// shader_hash: 00bdae7a717783ec01c1587d7fe7f16b
// constant_buffers: 1
// bound_resources: 1
// input_parameters: 2
// output_parameters: 4
// instruction_count: 90
// float_instruction_count: 14
// texture_load_instructions: 5
auto Shaders::fibers_light_vs() const -> std::span<const std::byte> {
    return std::span(_data).subspan(109528, 5488);
}

// shader_hash: 0d545b607d54aea303eb4a8db4cf31da
// input_parameters: 4
// output_parameters: 1
// instruction_count: 5
auto Shaders::fibers_light_ps() const -> std::span<const std::byte> {
    return std::span(_data).subspan(115016, 3144);
}

// shader_hash: 67ce76c1b8cef37afea87f3af4e63980
// constant_buffers: 1
// bound_resources: 1
// input_parameters: 2
// output_parameters: 5
// instruction_count: 74
// float_instruction_count: 8
// texture_load_instructions: 3
auto Shaders::fibers_plane_vs() const -> std::span<const std::byte> {
    return std::span(_data).subspan(118160, 5508);
}

// shader_hash: bf043e6ca1fc244f4228b30f10d7a497
// constant_buffers: 1
// bound_resources: 1
// input_parameters: 5
// output_parameters: 1
// instruction_count: 105
// dynamic_flow_control_count: 2
// float_instruction_count: 40
// int_instruction_count: 4
// texture_load_instructions: 5
auto Shaders::fibers_plane_ps() const -> std::span<const std::byte> {
    return std::span(_data).subspan(123668, 5928);
}

// shader_hash: 774cf19def534ea19fa2c04206753609
// input_parameters: 2
// output_parameters: 2
// instruction_count: 17
// float_instruction_count: 6
// uint_instruction_count: 3
auto Shaders::fibers_debug_vs() const -> std::span<const std::byte> {
    return std::span(_data).subspan(129596, 3256);
}

// shader_hash: fba11f278d24921065a7ff897de265ce
// constant_buffers: 1
// bound_resources: 1
// input_parameters: 2
// output_parameters: 1
// instruction_count: 44
// float_instruction_count: 11
// texture_normal_instructions: 1
// texture_load_instructions: 1
auto Shaders::fibers_debug_ps() const -> std::span<const std::byte> {
    return std::span(_data).subspan(132852, 4944);
}

// shader_hash: 22751d6713db53a95263c35d46c1dc99
// constant_buffers: 1
// bound_resources: 1
// input_parameters: 2
// output_parameters: 4
// instruction_count: 63
// float_instruction_count: 7
// texture_load_instructions: 3
auto Shaders::env_draw_vs() const -> std::span<const std::byte> {
    return std::span(_data).subspan(137796, 5116);
}

// shader_hash: 34759f4385b2a6b7410f8378fc3e6341
// constant_buffers: 1
// bound_resources: 1
// input_parameters: 4
// output_parameters: 1
// instruction_count: 20
// texture_normal_instructions: 1
auto Shaders::env_draw_ps() const -> std::span<const std::byte> {
    return std::span(_data).subspan(142912, 4444);
}

} // namespace fb::baked
