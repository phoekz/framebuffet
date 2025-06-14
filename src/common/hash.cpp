#include "hash.hpp"

#include <xxhash.h>

namespace fb {

auto hash128(Span<const std::byte> data) -> Hash128 {
    XXH128_hash_t hash = XXH3_128bits(data.data(), data.size());
    return std::bit_cast<Hash128>(hash);
}

} // namespace fb
