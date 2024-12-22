#include <common/common.hpp>

int main() {
    using namespace fb;

    // Time since process start.
    const auto time_since_process_start = fb::get_time_since_process_start();

    // Console.
    attach_console();

    // Log time since process start.
    FB_LOG_INFO("Time since process start: {} s", time_since_process_start);

    return 0;
}
