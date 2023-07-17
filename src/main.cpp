#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>
#include <SDL2/SDL_syswm.h>

#define WIN32_LEAN_AND_MEAN
#include <directx/d3dx12.h>
#include <dxgi1_6.h>

#include <D3D12MemAlloc.h>

#include <cstdio>

// Constants.
constexpr const char* WINDOW_TITLE = "framebuffet";
constexpr int WINDOW_WIDTH = 640;
constexpr int WINDOW_HEIGHT = 480;
constexpr D3D_FEATURE_LEVEL FEATURE_LEVEL = D3D_FEATURE_LEVEL_12_2;

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

bool CheckHResult(HRESULT hr, const char* msg) {
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
        return false;
    }
    return true;
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

    // Create D3D12 debug controller.
    ID3D12Debug* d3d12Debug = nullptr;
    {
        AssertHresult(D3D12GetDebugInterface(IID_PPV_ARGS(&d3d12Debug)), "D3D12GetDebugInterface");
        d3d12Debug->EnableDebugLayer();
        printf("ID3D12Debug: %p\n", (void*)d3d12Debug);
    }

    // Create DXGI factory.
    IDXGIFactory4* dxgiFactory = nullptr;
    {
        AssertHresult(CreateDXGIFactory2(0, IID_PPV_ARGS(&dxgiFactory)), "CreateDXGIFactory2");
        printf("IDXGIFactory4: %p\n", (void*)dxgiFactory);
    }

    // Find the best DXGI adapter.
    IDXGIAdapter1* dxgiAdapter = nullptr;
    {
        uint32_t bestAdapterIndex = UINT32_MAX;
        for (uint32_t adapterIndex = 0;
             dxgiFactory->EnumAdapters1(adapterIndex, &dxgiAdapter) != DXGI_ERROR_NOT_FOUND;
             ++adapterIndex) {
            DXGI_ADAPTER_DESC1 adapterDesc = {};
            AssertHresult(dxgiAdapter->GetDesc1(&adapterDesc), "IDXGIAdapter1::GetDesc1");

            // Ignore software adapters.
            if (adapterDesc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE) {
                continue;
            }

            // Check for support.
            if (CheckHResult(
                    D3D12CreateDevice(dxgiAdapter, FEATURE_LEVEL, __uuidof(ID3D12Device), nullptr),
                    "D3D12CreateDevice")) {
                printf("Found adapter %u: %ws\n", adapterIndex, adapterDesc.Description);
                bestAdapterIndex = adapterIndex;
                SafeRelease(dxgiAdapter);
                break;
            }
        }
        if (bestAdapterIndex == UINT32_MAX) {
            printf("error: no suitable adapter found\n");
            return 1;
        }
        dxgiFactory->EnumAdapters1(bestAdapterIndex, &dxgiAdapter);
        printf("IDXGIAdapter1: %p\n", (void*)dxgiAdapter);
    }

    // Create D3D12 device.
    ID3D12Device* d3d12Device = nullptr;
    {
        AssertHresult(
            D3D12CreateDevice(dxgiAdapter, FEATURE_LEVEL, IID_PPV_ARGS(&d3d12Device)),
            "D3D12CreateDevice");
        printf("ID3D12Device: %p\n", (void*)d3d12Device);
    }

    // Create D3D12MA allocator.
    D3D12MA::Allocator* d3d12Allocator = nullptr;
    {
        D3D12MA::ALLOCATOR_DESC allocatorDesc = {};
        allocatorDesc.pDevice = d3d12Device;
        allocatorDesc.pAdapter = dxgiAdapter;
        allocatorDesc.pAllocationCallbacks = nullptr;
        allocatorDesc.Flags = D3D12MA::ALLOCATOR_FLAG_NONE;
        AssertHresult(
            D3D12MA::CreateAllocator(&allocatorDesc, &d3d12Allocator),
            "D3D12MA::CreateAllocator");
        printf("D3D12MA::Allocator: %p\n", (void*)d3d12Allocator);
    }

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
    SafeRelease(d3d12Allocator);
    SafeRelease(d3d12Device);
    SafeRelease(dxgiAdapter);
    SafeRelease(dxgiFactory);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
