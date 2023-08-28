#include <common/common.hpp>
#include <kitchen/kitchen.hpp>

namespace fb {

//
// Constants.
//

inline constexpr std::string_view WINDOW_TITLE = "stockcube 😎"sv;
inline constexpr int WINDOW_WIDTH = 1280;
inline constexpr int WINDOW_HEIGHT = 832;

//
// Stockcube.
//

struct Stockcube {
    auto run() -> void;

    // Systems.
    Window window;
    GpuDevice device;
};

auto Stockcube::run() -> void {
    // Console.
    attach_console();

    // Init.
    {
        auto timer = Instant();
        window.create(Window::Desc {WINDOW_TITLE, WINDOW_WIDTH, WINDOW_HEIGHT});
        device.create(window);
        device.log_stats();
    }

    // Wait for pending GPU work to complete before running destructors.
    device.wait();
}

} // namespace fb

int main() {
    FB_LOG_INFO("sizeof(Stockcube): {} bytes", sizeof(fb::Stockcube));
    auto stockcube = std::make_unique<fb::Stockcube>();
    stockcube->run();
    return 0;
}
