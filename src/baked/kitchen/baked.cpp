#include "baked.hpp"

namespace fb::baked::kitchen {

#define texture_data(rp, sp, off, sz)                                                   \
    TextureData {                                                                       \
        .row_pitch = rp, .slice_pitch = sp, .data = transmuted_span<std::byte>(off, sz) \
    }

auto Assets::load() -> void {
    // hash: 799fc360204416196536a93c9eff68ae
    ZoneScoped;
    _data = read_whole_file("fb_kitchen_assets.bin");
    FB_ASSERT(_data.size() == 162588);
}

auto Assets::imgui_font() const -> Copy {
    return Copy {
        // hash: 799fc360204416196536a93c9eff68ae
        .data = transmuted_span<std::byte>(0, 162588),
    };
}

auto Shaders::load() -> void {
    // hash: c341ef8bae9f9a40b1a9fde91351e2fe
    ZoneScoped;
    _data = read_whole_file("fb_kitchen_shaders.bin");
    FB_ASSERT(_data.size() == 32124);
}

// shader_hash: 1a56b389138880d61871707f8f50b695
// constant_buffers: 1
// bound_resources: 1
// input_parameters: 2
// output_parameters: 3
// instruction_count: 86
// float_instruction_count: 31
// int_instruction_count: 1
// texture_load_instructions: 3
auto Shaders::gui_draw_vs() const -> std::span<const std::byte> {
    return std::span(_data).subspan(0, 5376);
}

// shader_hash: f56162255ee90a472ca6e4af5aec7f22
// constant_buffers: 1
// bound_resources: 1
// input_parameters: 3
// output_parameters: 1
// instruction_count: 28
// float_instruction_count: 4
// texture_normal_instructions: 1
auto Shaders::gui_draw_ps() const -> std::span<const std::byte> {
    return std::span(_data).subspan(5376, 4436);
}

// shader_hash: a1ad702e1d13ee5a5a0cd09356116e1f
// constant_buffers: 1
// bound_resources: 1
// input_parameters: 2
// output_parameters: 2
// instruction_count: 74
// float_instruction_count: 16
// texture_load_instructions: 2
auto Shaders::debug_draw_draw_vs() const -> std::span<const std::byte> {
    return std::span(_data).subspan(9812, 5140);
}

// shader_hash: fd99ba74a11c0d4a0da57f344f133451
// input_parameters: 2
// output_parameters: 1
// instruction_count: 9
auto Shaders::debug_draw_draw_ps() const -> std::span<const std::byte> {
    return std::span(_data).subspan(14952, 3076);
}

// shader_hash: accf9e643c113fcb4e7d227c723bc66f
// constant_buffers: 1
// bound_resources: 1
// instruction_count: 1533
// temp_array_count: 2048
// dynamic_flow_control_count: 30
// array_instruction_count: 76
// float_instruction_count: 449
// int_instruction_count: 347
// uint_instruction_count: 122
// texture_normal_instructions: 4
// texture_load_instructions: 16
// barrier_instructions: 9
// interlocked_instructions: 1
// texture_store_instructions: 22
auto Shaders::spd_downsample_cs() const -> std::span<const std::byte> {
    return std::span(_data).subspan(18028, 14096);
}

#undef texture_data

} // namespace fb::baked::kitchen
