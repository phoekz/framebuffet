#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>
#include <SDL2/SDL_syswm.h>

#define WIN32_LEAN_AND_MEAN
#include <directx/d3d12.h>
#include <dxgi1_6.h>

#include <cstdio>

// Constants.
constexpr const char* WINDOW_TITLE = "framebuffet";
constexpr int WINDOW_WIDTH = 640;
constexpr int WINDOW_HEIGHT = 480;

// Utility functions.
template<typename T>
void SafeRelease(T*& ptr) {
    if (ptr != nullptr) {
        ptr->Release();
        ptr = nullptr;
    }
}

void AssertHresult(HRESULT hr, const char* msg) {
    if (FAILED(hr)) {
        wchar_t wsz[1024];
        FormatMessageW(
            FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
            nullptr,
            hr,
            0,
            wsz,
            sizeof(wsz) / sizeof(wsz[0]),
            nullptr);
        printf("error: %s failed with HRESULT 0x%08lx\nmessage: %ws\n", msg, hr, wsz);
        exit(1);
    }
}

// Main function.
int main() {
    // Initialize SDL.
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
        printf("error initializing SDL: %s\n", SDL_GetError());
        return 1;
    }

    // Create a window.
    SDL_Window* window = SDL_CreateWindow(
        WINDOW_TITLE,
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        WINDOW_WIDTH,
        WINDOW_HEIGHT,
        0);
    if (window == NULL) {
        printf("error creating window: %s\n", SDL_GetError());
        return 1;
    }

    // Get HWND from SDL_Window.
    HWND hwnd = nullptr;
    {
        SDL_SysWMinfo info;
        SDL_VERSION(&info.version);
        SDL_GetWindowWMInfo(window, &info);
        hwnd = info.info.win.window;
        printf("HWND: %p\n", (void*)hwnd);
    }

    // Get DXGI factory.
    IDXGIFactory4* dxgiFactory = nullptr;
    AssertHresult(CreateDXGIFactory2(0, IID_PPV_ARGS(&dxgiFactory)), "CreateDXGIFactory2");
    printf("IDXGIFactory4: %p\n", (void*)dxgiFactory);

    // Main loop.
    bool done = false;
    while (!done) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                done = true;
            } else if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE) {
                done = true;
            }
        }
    }

    // Cleanup.
    SafeRelease(dxgiFactory);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
