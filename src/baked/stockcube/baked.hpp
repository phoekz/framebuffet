#pragma once

#include "../baked_types.hpp"

namespace fb::baked::stockcube {

class Assets {
public:
    auto load() -> void;

    auto farm_field_hdr_texture() const -> Texture;
    auto winter_evening_hdr_texture() const -> Texture;
    auto shanghai_bund_hdr_texture() const -> Texture;
    auto industrial_sunset_02_puresky_hdr_texture() const -> Texture;
    auto skybox_mesh() const -> Mesh;
    auto sphere_mesh() const -> Mesh;

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

    auto cfr_cs() const -> Span<const std::byte>;
    auto lut_cs() const -> Span<const std::byte>;
    auto irr_acc_cs() const -> Span<const std::byte>;
    auto irr_div_cs() const -> Span<const std::byte>;
    auto rad_acc_cs() const -> Span<const std::byte>;
    auto rad_div_cs() const -> Span<const std::byte>;
    auto bg_vs() const -> Span<const std::byte>;
    auto bg_ps() const -> Span<const std::byte>;
    auto model_vs() const -> Span<const std::byte>;
    auto model_ps() const -> Span<const std::byte>;
    auto screen_vs() const -> Span<const std::byte>;
    auto screen_ps() const -> Span<const std::byte>;
    auto blit_vs() const -> Span<const std::byte>;
    auto blit_ps() const -> Span<const std::byte>;

private:
    FileBuffer _file;
};

} // namespace fb::baked::stockcube
