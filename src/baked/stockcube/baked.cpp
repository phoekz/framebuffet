#include "baked.hpp"

namespace fb::baked::stockcube {

Assets::Assets() {
    // hash: a0650c10da39e6dca36f015fd81ce0a3
    _data = read_whole_file("fb_stockcube_assets.bin");
    FB_ASSERT(_data.size() == 100817472);
}

auto Assets::farm_field_hdr_texture() const -> Texture {
    decltype(Texture::datas) datas = {};
    datas[0] = TextureData {
        // mip_level: 0
        // width: 2048
        // height: 1024
        .row_pitch = 32768,
        .slice_pitch = 33554432,
        // hash: 99b1db0825065596911e1fbe8a209f04
        .data = transmuted_span<std::byte>(0, 33554432),
    };
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
    datas[0] = TextureData {
        // mip_level: 0
        // width: 2048
        // height: 1024
        .row_pitch = 32768,
        .slice_pitch = 33554432,
        // hash: 3a756371dff470416ac497492fe79829
        .data = transmuted_span<std::byte>(33554432, 33554432),
    };
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
    datas[0] = TextureData {
        // mip_level: 0
        // width: 2048
        // height: 1024
        .row_pitch = 32768,
        .slice_pitch = 33554432,
        // hash: 61da5f094312e27801f5f466a719d765
        .data = transmuted_span<std::byte>(67108864, 33554432),
    };
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
        .vertices = transmuted_span<Vertex>(100663296, 24),
        // hash: 0bbe6ec9d4b61b792981857b935d2c96
        .indices = transmuted_span<Index>(100664448, 36),
    };
}

auto Assets::sphere_mesh() const -> Mesh {
    // vertex_count: 2145
    // face_count: 4160
    return Mesh {
        // hash: bdea0aeb8805424705ca957338103a57
        .vertices = transmuted_span<Vertex>(100664592, 2145),
        // hash: b69e987626eec7de0864668ffde28fbe
        .indices = transmuted_span<Index>(100767552, 12480),
    };
}

Shaders::Shaders() {
    // hash: 0787184c51b0259b0a9ac13871d66df3
    _data = read_whole_file("fb_stockcube_shaders.bin");
    FB_ASSERT(_data.size() == 65836);
}

// shader_hash: 4a07e1efdba1af65b24e576d42c8ef71
// constant_buffers: 1
// bound_resources: 1
// instruction_count: 77
// float_instruction_count: 32
// uint_instruction_count: 4
// texture_normal_instructions: 1
// texture_store_instructions: 1
auto Shaders::cfr_cs() const -> std::span<const std::byte> {
    return std::span(_data).subspan(0, 4260);
}

// shader_hash: 9450109bf2d9031d2076d9f83b47c182
// constant_buffers: 1
// bound_resources: 1
// instruction_count: 147
// dynamic_flow_control_count: 3
// float_instruction_count: 88
// int_instruction_count: 3
// uint_instruction_count: 23
// texture_store_instructions: 1
auto Shaders::lut_cs() const -> std::span<const std::byte> {
    return std::span(_data).subspan(4260, 4564);
}

// shader_hash: f3bc567f75b51a3357d0c2d8bd15331d
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
    return std::span(_data).subspan(8824, 5572);
}

// shader_hash: b2501e1a79251e26fea5e8cad845cb07
// constant_buffers: 1
// bound_resources: 1
// instruction_count: 33
// float_instruction_count: 6
// int_instruction_count: 2
// texture_load_instructions: 1
// texture_store_instructions: 1
auto Shaders::irr_div_cs() const -> std::span<const std::byte> {
    return std::span(_data).subspan(14396, 3732);
}

// shader_hash: 5fef57ae9c5c891b8c01db44940540e9
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
    return std::span(_data).subspan(18128, 6116);
}

// shader_hash: 3a790e9fd4189adf70e2c72d602909a6
// constant_buffers: 1
// bound_resources: 1
// instruction_count: 44
// dynamic_flow_control_count: 1
// float_instruction_count: 6
// int_instruction_count: 5
// texture_load_instructions: 1
// texture_store_instructions: 1
auto Shaders::rad_div_cs() const -> std::span<const std::byte> {
    return std::span(_data).subspan(24244, 3864);
}

// shader_hash: 51857574d4bcba8064d6677d30b8e0f7
// constant_buffers: 1
// bound_resources: 1
// input_parameters: 2
// output_parameters: 4
// instruction_count: 62
// float_instruction_count: 6
// texture_load_instructions: 3
auto Shaders::bg_vs() const -> std::span<const std::byte> {
    return std::span(_data).subspan(28108, 5104);
}

// shader_hash: 7270dcdaf8185f6cc2f5050fa15cf431
// constant_buffers: 1
// bound_resources: 1
// input_parameters: 4
// output_parameters: 1
// instruction_count: 31
// float_instruction_count: 2
// int_instruction_count: 1
// texture_normal_instructions: 1
auto Shaders::bg_ps() const -> std::span<const std::byte> {
    return std::span(_data).subspan(33212, 4680);
}

// shader_hash: 1abc8265f905cb71eabf0b99486f5b3b
// constant_buffers: 1
// bound_resources: 1
// input_parameters: 2
// output_parameters: 3
// instruction_count: 65
// float_instruction_count: 8
// texture_load_instructions: 2
auto Shaders::model_vs() const -> std::span<const std::byte> {
    return std::span(_data).subspan(37892, 5032);
}

// shader_hash: 545e76afcea1653fe396155c18687dcc
// constant_buffers: 1
// bound_resources: 1
// input_parameters: 3
// output_parameters: 1
// instruction_count: 89
// float_instruction_count: 39
// texture_normal_instructions: 3
auto Shaders::model_ps() const -> std::span<const std::byte> {
    return std::span(_data).subspan(42924, 5268);
}

// shader_hash: 857aab9d7f016524e23b0946f70544ab
// constant_buffers: 1
// bound_resources: 1
// input_parameters: 2
// output_parameters: 6
// instruction_count: 76
// float_instruction_count: 12
// texture_load_instructions: 8
auto Shaders::screen_vs() const -> std::span<const std::byte> {
    return std::span(_data).subspan(48192, 5604);
}

// shader_hash: 31d6c9509125fa76c87a301098cc24b2
// input_parameters: 6
// output_parameters: 1
// instruction_count: 21
// float_instruction_count: 2
// texture_normal_instructions: 1
auto Shaders::screen_ps() const -> std::span<const std::byte> {
    return std::span(_data).subspan(53796, 4140);
}

// shader_hash: c5f7f57635d605d37e17fa9cf500a28e
// input_parameters: 2
// output_parameters: 2
// instruction_count: 17
// float_instruction_count: 6
// uint_instruction_count: 3
auto Shaders::blit_vs() const -> std::span<const std::byte> {
    return std::span(_data).subspan(57936, 3228);
}

// shader_hash: 9df73e4ad6c2f92af8c7ed0787e93bfb
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
    return std::span(_data).subspan(61164, 4672);
}

} // namespace fb::baked::stockcube
