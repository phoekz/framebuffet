inline constexpr std::string_view BAKED_HPP = R"(#pragma once

    #include "../baked_types.hpp"

    namespace fb::baked::{{app_name}} {

    class Assets {
    public:
        auto load() -> void;

        {{asset_decls}}

    private:
        template<typename T>
        auto transmuted_span(size_t offset, size_t element_count) const -> Span<const T> {
            return Span<const T>((const T*)(_file.bytes() + offset), element_count);
        }

        FileBuffer _file;
    };

    class Shaders {
    public:
        auto load() -> void;

        {{shader_decls}}

    private:
        FileBuffer _file;
    };

    } // namespace fb::baked::{{app_name}}
)"sv;
