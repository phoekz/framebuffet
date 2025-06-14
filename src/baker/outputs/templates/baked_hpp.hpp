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
            return Span((const T*)(_data.data() + offset), element_count);
        }

        std::vector<std::byte> _data;
    };

    class Shaders {
    public:
        auto load() -> void;

        {{shader_decls}}

    private:
        std::vector<std::byte> _data;
    };

    } // namespace fb::baked::{{app_name}}
)"sv;
