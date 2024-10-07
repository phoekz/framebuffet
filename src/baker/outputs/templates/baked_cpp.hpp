inline constexpr std::string_view BAKED_CPP = R"(#include "baked.hpp"

    namespace fb::baked::{{app_name}} {

    #define texture_data(rp, sp, off, sz) TextureData { .row_pitch = rp, .slice_pitch = sp, .data = transmuted_span<std::byte>(off, sz) }

    auto Assets::load() -> void {
        // hash: {{assets_bin_hash}}
        FB_PERF_FUNC();
        _data = read_whole_file("fb_{{app_name}}_assets.bin");
        FB_ASSERT(_data.size() == {{assets_byte_count}});
    }

    {{asset_defns}}

    auto Shaders::load() -> void {
        // hash: {{shaders_bin_hash}}
        FB_PERF_FUNC();
        _data = read_whole_file("fb_{{app_name}}_shaders.bin");
        FB_ASSERT(_data.size() == {{shaders_byte_count}});
    }

    {{shader_defns}}

    #undef texture_data

    } // namespace fb::baked::{{app_name}}
)"sv;
