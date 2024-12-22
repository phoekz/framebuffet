#include "common.hpp"

#include <nlohmann/json.hpp>

namespace fb::techniques {

auto write_output_metadata_file(std::string_view path, const OutputMetadata& meta) -> void {
    const ord_json json = {
        {"format", meta.format},
        {"unit_byte_count", meta.unit_byte_count},
        {"width", meta.width},
        {"height", meta.height},
        {"depth", meta.depth},
        {"mip_count", meta.mip_count},
    };
    const auto json_str = json.dump(4);
    const auto json_bytes = std::as_bytes(std::span(json_str));
    write_whole_file(path, json_bytes);
}

} // namespace fb::techniques
