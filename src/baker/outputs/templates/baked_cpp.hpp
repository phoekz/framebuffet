inline constexpr std::string_view BAKED_CPP = R"(#include "baked.hpp"

    namespace fb::baked::{{app_name}} {

    Assets::Assets() {
        // hash: {{assets_bin_hash}}
        _data = read_whole_file("fb_{{app_name}}_assets.bin");
        FB_ASSERT(_data.size() == {{assets_total_size}});
    }

    {{asset_defns}}

    Shaders::Shaders() {
        // hash: {{shaders_bin_hash}}
        _data = read_whole_file("fb_{{app_name}}_shaders.bin");
        FB_ASSERT(_data.size() == {{shaders_total_size}});
    }

    {{shader_defns}}

    } // namespace fb::baked::{{app_name}}
)"sv;