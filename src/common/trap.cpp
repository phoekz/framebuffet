#include "trap.hpp"
#include "perf.hpp"

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
    FB_PERF_TRACY_ALLOC(ptr, count);
    return ptr;
}

auto operator delete(void* ptr) noexcept -> void {
    FB_PERF_TRACY_FREE(ptr);
    free(ptr);
}
