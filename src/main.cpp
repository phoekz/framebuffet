#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>
#include <SDL2/SDL_syswm.h>

#define WIN32_LEAN_AND_MEAN
#include <directx/d3d12sdklayers.h>
#include <directx/d3dx12.h>
#include <dxgi1_6.h>
#include <dxgidebug.h>

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
    ID3D12Debug6* d3d12Debug = nullptr;
    {
        AssertHresult(D3D12GetDebugInterface(IID_PPV_ARGS(&d3d12Debug)), "D3D12GetDebugInterface");
        d3d12Debug->EnableDebugLayer();
        d3d12Debug->SetEnableSynchronizedCommandQueueValidation(true);
        printf("ID3D12Debug: %p\n", (void*)d3d12Debug);
    }

    // Create DXGI info queue.
    IDXGIInfoQueue* dxgiInfoQueue = nullptr;
    {
        AssertHresult(
            DXGIGetDebugInterface1(0, IID_PPV_ARGS(&dxgiInfoQueue)),
            "DXGIGetDebugInterface1");
        dxgiInfoQueue->SetBreakOnSeverity(
            DXGI_DEBUG_ALL,
            DXGI_INFO_QUEUE_MESSAGE_SEVERITY_WARNING,
            true);
        dxgiInfoQueue->SetBreakOnSeverity(
            DXGI_DEBUG_ALL,
            DXGI_INFO_QUEUE_MESSAGE_SEVERITY_ERROR,
            true);
        dxgiInfoQueue->SetBreakOnSeverity(
            DXGI_DEBUG_ALL,
            DXGI_INFO_QUEUE_MESSAGE_SEVERITY_CORRUPTION,
            true);
        printf("IDXGIInfoQueue: %p\n", (void*)dxgiInfoQueue);
    }

    // Create DXGI factory.
    IDXGIFactory4* dxgiFactory = nullptr;
    {
        AssertHresult(
            CreateDXGIFactory2(DXGI_CREATE_FACTORY_DEBUG, IID_PPV_ARGS(&dxgiFactory)),
            "CreateDXGIFactory2");
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
    ID3D12Device9* d3d12Device9 = nullptr;
    {
        AssertHresult(
            D3D12CreateDevice(dxgiAdapter, FEATURE_LEVEL, IID_PPV_ARGS(&d3d12Device)),
            "D3D12CreateDevice");
        d3d12Device->QueryInterface(IID_PPV_ARGS(&d3d12Device9));
        printf("ID3D12Device: %p\n", (void*)d3d12Device);
        printf("ID3D12Device9: %p\n", (void*)d3d12Device9);
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

    // Create D3D12 command queue.
    ID3D12CommandQueue* d3d12CommandQueue = nullptr;
    {
        D3D12_COMMAND_QUEUE_DESC commandQueueDesc = {};
        commandQueueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
        commandQueueDesc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
        commandQueueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
        commandQueueDesc.NodeMask = 0;
        AssertHresult(
            d3d12Device->CreateCommandQueue(&commandQueueDesc, IID_PPV_ARGS(&d3d12CommandQueue)),
            "ID3D12Device::CreateCommandQueue");
        printf("ID3D12CommandQueue: %p\n", (void*)d3d12CommandQueue);
    }

    // Create DXGI swap chain.
    IDXGISwapChain4* dxgiSwapChain = nullptr;
    {
        DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
        swapChainDesc.Width = WINDOW_WIDTH;
        swapChainDesc.Height = WINDOW_HEIGHT;
        swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        swapChainDesc.Stereo = FALSE;
        swapChainDesc.SampleDesc.Count = 1;
        swapChainDesc.SampleDesc.Quality = 0;
        swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        swapChainDesc.BufferCount = 2;
        swapChainDesc.Scaling = DXGI_SCALING_STRETCH;
        swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;
        swapChainDesc.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;
        swapChainDesc.Flags = 0;
        IDXGISwapChain1* swapChain = nullptr;
        AssertHresult(
            dxgiFactory->CreateSwapChainForHwnd(
                d3d12CommandQueue,
                hwnd,
                &swapChainDesc,
                nullptr,
                nullptr,
                &swapChain),
            "IDXGIFactory4::CreateSwapChainForHwnd");
        swapChain->QueryInterface(IID_PPV_ARGS(&dxgiSwapChain));
        SafeRelease(swapChain);
        printf("IDXGISwapChain4: %p\n", (void*)dxgiSwapChain);
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
    SafeRelease(dxgiSwapChain);
    SafeRelease(d3d12CommandQueue);
    SafeRelease(d3d12Allocator);
    SafeRelease(d3d12Device9);
    SafeRelease(d3d12Device);
    SafeRelease(dxgiAdapter);
    SafeRelease(dxgiInfoQueue);
    SafeRelease(dxgiFactory);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
