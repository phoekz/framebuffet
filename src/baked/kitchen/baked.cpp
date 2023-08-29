#include "baked.hpp"

namespace fb::baked::kitchen {

Assets::Assets() {
    // hash: 799fc360204416196536a93c9eff68ae
    _data = read_whole_file("fb_kitchen_assets.bin");
    FB_ASSERT(_data.size() == 162588);
}

auto Assets::imgui_font() const -> Copy {
    return Copy {
        // hash: 799fc360204416196536a93c9eff68ae
        .data = transmuted_span<std::byte>(0, 162588),
    };
}

Shaders::Shaders() {
    // hash: de84d336b259beb06a3220d54b501055
    _data = read_whole_file("fb_kitchen_shaders.bin");
    FB_ASSERT(_data.size() == 31808);
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
    return std::span(_data).subspan(17944, 13864);
}

} // namespace fb::baked::kitchen
