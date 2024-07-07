#include "baked.hpp"

namespace fb::baked::stockcube {

#define texture_data(rp, sp, off, sz)                                                   \
    TextureData {                                                                       \
        .row_pitch = rp, .slice_pitch = sp, .data = transmuted_span<std::byte>(off, sz) \
    }

auto Assets::load() -> void {
    // hash: 7f09e67f65e57037f74b249ecf51e6bd
    ZoneScoped;
    _data = read_whole_file("fb_stockcube_assets.bin");
    FB_ASSERT(_data.size() == 134371904);
}

auto Assets::farm_field_hdr_texture() const -> Texture {
    decltype(Texture::datas) datas = {};
    // clang-format off
    datas[ 0] = texture_data(32768, 33554432,         0, 33554432); // hash: 99b1db0825065596911e1fbe8a209f04, width: 2048, height: 1024
    // clang-format on
    return Texture {
        .format = DXGI_FORMAT_R32G32B32A32_FLOAT,
        .width = 2048,
        .height = 1024,
        .mip_count = 1,
        .datas = datas,
    };
}

auto Assets::winter_evening_hdr_texture() const -> Texture {
    decltype(Texture::datas) datas = {};
    // clang-format off
    datas[ 0] = texture_data(32768, 33554432,  33554432, 33554432); // hash: 3a756371dff470416ac497492fe79829, width: 2048, height: 1024
    // clang-format on
    return Texture {
        .format = DXGI_FORMAT_R32G32B32A32_FLOAT,
        .width = 2048,
        .height = 1024,
        .mip_count = 1,
        .datas = datas,
    };
}

auto Assets::shanghai_bund_hdr_texture() const -> Texture {
    decltype(Texture::datas) datas = {};
    // clang-format off
    datas[ 0] = texture_data(32768, 33554432,  67108864, 33554432); // hash: 61da5f094312e27801f5f466a719d765, width: 2048, height: 1024
    // clang-format on
    return Texture {
        .format = DXGI_FORMAT_R32G32B32A32_FLOAT,
        .width = 2048,
        .height = 1024,
        .mip_count = 1,
        .datas = datas,
    };
}

auto Assets::industrial_sunset_02_puresky_hdr_texture() const -> Texture {
    decltype(Texture::datas) datas = {};
    // clang-format off
    datas[ 0] = texture_data(32768, 33554432, 100663296, 33554432); // hash: 40f641d2aaba7e8800dcf4e8cc0e0274, width: 2048, height: 1024
    // clang-format on
    return Texture {
        .format = DXGI_FORMAT_R32G32B32A32_FLOAT,
        .width = 2048,
        .height = 1024,
        .mip_count = 1,
        .datas = datas,
    };
}

auto Assets::skybox_mesh() const -> Mesh {
    // vertex_count: 24
    // face_count: 12
    return Mesh {
        // hash: 952f25941581c525b7890e0284fd3b52
        .vertices = transmuted_span<Vertex>(134217728, 24),
        // hash: 0bbe6ec9d4b61b792981857b935d2c96
        .indices = transmuted_span<Index>(134218880, 36),
    };
}

auto Assets::sphere_mesh() const -> Mesh {
    // vertex_count: 2145
    // face_count: 4160
    return Mesh {
        // hash: bdea0aeb8805424705ca957338103a57
        .vertices = transmuted_span<Vertex>(134219024, 2145),
        // hash: b69e987626eec7de0864668ffde28fbe
        .indices = transmuted_span<Index>(134321984, 12480),
    };
}

auto Shaders::load() -> void {
    // hash: 11c888a71c1fae5c9c54c9f3d7355beb
    ZoneScoped;
    _data = read_whole_file("fb_stockcube_shaders.bin");
    FB_ASSERT(_data.size() == 66036);
}

// shader_hash: d4277363486ca3578c07a22a4024680b
// constant_buffers: 1
// bound_resources: 1
// instruction_count: 77
// float_instruction_count: 32
// uint_instruction_count: 4
// texture_normal_instructions: 1
// texture_store_instructions: 1
auto Shaders::cfr_cs() const -> std::span<const std::byte> {
    return std::span(_data).subspan(0, 4268);
}

// shader_hash: 2f1d1e53c6e1cbca5e303f148017add0
// constant_buffers: 1
// bound_resources: 1
// instruction_count: 147
// dynamic_flow_control_count: 3
// float_instruction_count: 88
// int_instruction_count: 3
// uint_instruction_count: 23
// texture_store_instructions: 1
auto Shaders::lut_cs() const -> std::span<const std::byte> {
    return std::span(_data).subspan(4268, 4572);
}

// shader_hash: a6d8db82a47f080c40fc68bee34f7561
// constant_buffers: 1
// bound_resources: 1
// instruction_count: 235
// dynamic_flow_control_count: 4
// float_instruction_count: 141
// int_instruction_count: 6
// uint_instruction_count: 23
// texture_normal_instructions: 1
// texture_load_instructions: 1
// texture_store_instructions: 2
auto Shaders::irr_acc_cs() const -> std::span<const std::byte> {
    return std::span(_data).subspan(8840, 5588);
}

// shader_hash: cb848a5af58ac55bdef9ce8a7f43fdce
// constant_buffers: 1
// bound_resources: 1
// instruction_count: 33
// float_instruction_count: 6
// int_instruction_count: 2
// texture_load_instructions: 1
// texture_store_instructions: 1
auto Shaders::irr_div_cs() const -> std::span<const std::byte> {
    return std::span(_data).subspan(14428, 3744);
}

// shader_hash: 404cc87a0cdab1f33b5ffaa788225d0a
// constant_buffers: 1
// bound_resources: 1
// instruction_count: 297
// dynamic_flow_control_count: 10
// float_instruction_count: 167
// int_instruction_count: 11
// uint_instruction_count: 29
// texture_normal_instructions: 2
// texture_load_instructions: 1
// texture_store_instructions: 3
auto Shaders::rad_acc_cs() const -> std::span<const std::byte> {
    return std::span(_data).subspan(18172, 6132);
}

// shader_hash: b4b57014dba716bde3d03360d4388809
// constant_buffers: 1
// bound_resources: 1
// instruction_count: 44
// dynamic_flow_control_count: 1
// float_instruction_count: 6
// int_instruction_count: 5
// texture_load_instructions: 1
// texture_store_instructions: 1
auto Shaders::rad_div_cs() const -> std::span<const std::byte> {
    return std::span(_data).subspan(24304, 3876);
}

// shader_hash: 65cd8c2a779bb4dfda25b20192bee578
// constant_buffers: 1
// bound_resources: 1
// input_parameters: 2
// output_parameters: 4
// instruction_count: 62
// float_instruction_count: 6
// texture_load_instructions: 3
auto Shaders::bg_vs() const -> std::span<const std::byte> {
    return std::span(_data).subspan(28180, 5120);
}

// shader_hash: 0e0482c316f041b4bebc9942264b52e4
// constant_buffers: 1
// bound_resources: 1
// input_parameters: 4
// output_parameters: 1
// instruction_count: 31
// float_instruction_count: 2
// int_instruction_count: 1
// texture_normal_instructions: 1
auto Shaders::bg_ps() const -> std::span<const std::byte> {
    return std::span(_data).subspan(33300, 4696);
}

// shader_hash: 4c9579dd8bb09ea78354e64341064c74
// constant_buffers: 1
// bound_resources: 1
// input_parameters: 2
// output_parameters: 3
// instruction_count: 65
// float_instruction_count: 8
// texture_load_instructions: 2
auto Shaders::model_vs() const -> std::span<const std::byte> {
    return std::span(_data).subspan(37996, 5048);
}

// shader_hash: 3ca5762c204f8c5e433c101f0023f96c
// constant_buffers: 1
// bound_resources: 1
// input_parameters: 3
// output_parameters: 1
// instruction_count: 89
// float_instruction_count: 39
// texture_normal_instructions: 3
auto Shaders::model_ps() const -> std::span<const std::byte> {
    return std::span(_data).subspan(43044, 5288);
}

// shader_hash: c02dffcbc09c806460b8bfab37ebe785
// constant_buffers: 1
// bound_resources: 1
// input_parameters: 2
// output_parameters: 6
// instruction_count: 76
// float_instruction_count: 12
// texture_load_instructions: 8
auto Shaders::screen_vs() const -> std::span<const std::byte> {
    return std::span(_data).subspan(48332, 5620);
}

// shader_hash: 95db4a2ae02b74a6f13d030705b3cbe5
// input_parameters: 6
// output_parameters: 1
// instruction_count: 21
// float_instruction_count: 2
// texture_normal_instructions: 1
auto Shaders::screen_ps() const -> std::span<const std::byte> {
    return std::span(_data).subspan(53952, 4156);
}

// shader_hash: 2886ee06f6b91aaf0d306d345dbb615f
// input_parameters: 2
// output_parameters: 2
// instruction_count: 17
// float_instruction_count: 6
// uint_instruction_count: 3
auto Shaders::blit_vs() const -> std::span<const std::byte> {
    return std::span(_data).subspan(58108, 3244);
}

// shader_hash: e491d316a70d08ff74ffa04bb148e0cb
// constant_buffers: 1
// bound_resources: 1
// input_parameters: 2
// output_parameters: 1
// instruction_count: 62
// dynamic_flow_control_count: 1
// float_instruction_count: 30
// int_instruction_count: 1
// texture_normal_instructions: 1
auto Shaders::blit_ps() const -> std::span<const std::byte> {
    return std::span(_data).subspan(61352, 4684);
}

#undef texture_data

} // namespace fb::baked::stockcube
