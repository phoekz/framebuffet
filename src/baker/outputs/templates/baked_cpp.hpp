inline constexpr std::string_view BAKED_CPP = R"(#include "baked.hpp"

    namespace fb::baked::{{app_name}} {

    #define texture_data(rp, sp, off, sz) TextureData { .row_pitch = rp, .slice_pitch = sp, .data = transmuted_span<std::byte>(off, sz) }

    Assets::Assets() {
        // hash: {{assets_bin_hash}}
        _data = read_whole_file("fb_{{app_name}}_assets.bin");
        FB_ASSERT(_data.size() == {{assets_byte_count}});
    }

    {{asset_defns}}

    Shaders::Shaders() {
        // hash: {{shaders_bin_hash}}
        _data = read_whole_file("fb_{{app_name}}_shaders.bin");
        FB_ASSERT(_data.size() == {{shaders_byte_count}});
    }

    {{shader_defns}}

    #undef texture_data

    } // namespace fb::baked::{{app_name}}
)"sv;
