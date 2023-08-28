inline constexpr std::string_view BAKED_HPP = R"(#pragma once

    #include "../baked_types.hpp"

    namespace fb::baked::{{app_name}} {

    class Assets {
    public:
        Assets();

        {{asset_decls}}

    private:
        template<typename T>
        auto transmuted_span(size_t offset, size_t element_size) const -> std::span<const T> {
            return std::span((const T*)(_data.data() + offset), element_size);
        }

        std::vector<std::byte> _data;
    };

    class Shaders {
    public:
        Shaders();

        {{shader_decls}}

    private:
        std::vector<std::byte> _data;
    };

    } // namespace fb::baked::{{app_name}}
)"sv;
