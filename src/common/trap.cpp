#include "trap.hpp"

namespace fb {

bool FRAME_ALLOCATION_TRAP = false;

} // namespace fb

auto operator new(std::size_t count) -> void* {
    if (fb::FRAME_ALLOCATION_TRAP) {
        if (IsDebuggerPresent()) {
            __debugbreak();
        }
        std::quick_exit(1);
    }
    void* ptr = malloc(count);
    TracyAlloc(ptr, count);
    return ptr;
}

auto operator delete(void* ptr) noexcept -> void {
    TracyFree(ptr);
    free(ptr);
}
