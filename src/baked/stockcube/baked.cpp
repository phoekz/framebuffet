#include "baked.hpp"

namespace fb::baked::stockcube {

Assets::Assets() {
    // hash: 3f683cba3d4eb73fb84253a5907103e5
    _data = read_whole_file("fb_stockcube_assets.bin");
    FB_ASSERT(_data.size() == 67261744);
}

auto Assets::farm_field_hdr_texture() const -> Texture {
    return Texture {
        .format = DXGI_FORMAT_R32G32B32A32_FLOAT,
        .width = 2048,
        .height = 1024,
        .channel_count = 4,
        .mip_count = 1,
        .datas =
            {
                TextureData {
                    // mip_level: 0
                    // width: 2048
                    // height: 1024
                    .row_pitch = 32768,
                    .slice_pitch = 33554432,
                    // hash: 99b1db0825065596911e1fbe8a209f04
                    .data = transmuted_span<std::byte>(0, 33554432),
                },
            },
    };
}

auto Assets::winter_evening_hdr_texture() const -> Texture {
    return Texture {
        .format = DXGI_FORMAT_R32G32B32A32_FLOAT,
        .width = 2048,
        .height = 1024,
        .channel_count = 4,
        .mip_count = 1,
        .datas =
            {
                TextureData {
                    // mip_level: 0
                    // width: 2048
                    // height: 1024
                    .row_pitch = 32768,
                    .slice_pitch = 33554432,
                    // hash: 3a756371dff470416ac497492fe79829
                    .data = transmuted_span<std::byte>(33554432, 33554432),
                },
            },
    };
}

auto Assets::sphere_mesh() const -> Mesh {
    // vertex_count: 2145
    // face_count: 4160
    return Mesh {
        // hash: bdea0aeb8805424705ca957338103a57
        .vertices = transmuted_span<Vertex>(67108864, 2145),
        // hash: b69e987626eec7de0864668ffde28fbe
        .indices = transmuted_span<Index>(67211824, 12480),
    };
}

Shaders::Shaders() {
    // hash: b548ef34ce8997978ed7b20dd4cba37f
    _data = read_whole_file("fb_stockcube_shaders.bin");
    FB_ASSERT(_data.size() == 16364);
}

// shader_hash: 642b60e466588d32e27d47a06bc7bf5c
// constant_buffers: 1
// bound_resources: 1
// input_parameters: 2
// output_parameters: 3
// instruction_count: 67
// float_instruction_count: 12
// texture_load_instructions: 5
auto Shaders::screen_draw_vs() const -> std::span<const std::byte> {
    return std::span(_data).subspan(0, 5196);
}

// shader_hash: 961ec2997afb3a6f3d346efa194f92a1
// input_parameters: 3
// output_parameters: 1
// instruction_count: 17
// texture_normal_instructions: 1
auto Shaders::screen_draw_ps() const -> std::span<const std::byte> {
    return std::span(_data).subspan(5196, 3788);
}

// shader_hash: 248a48de0aa62893f6dcd0913b269c08
// input_parameters: 2
// output_parameters: 2
// instruction_count: 17
// float_instruction_count: 6
// uint_instruction_count: 3
auto Shaders::blit_draw_vs() const -> std::span<const std::byte> {
    return std::span(_data).subspan(8984, 3252);
}

// shader_hash: 094a3c0a7814839acba1972461c8c0bd
// constant_buffers: 1
// bound_resources: 1
// input_parameters: 2
// output_parameters: 1
// instruction_count: 19
// texture_normal_instructions: 1
auto Shaders::blit_draw_ps() const -> std::span<const std::byte> {
    return std::span(_data).subspan(12236, 4128);
}

} // namespace fb::baked::stockcube
