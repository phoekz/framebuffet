include(FetchContent)

# Windows Implementation Library (WIL).
message(STATUS "Fetching WIL...")
FetchContent_Declare(
    wil
    GIT_REPOSITORY https://github.com/microsoft/wil
)
option(WIL_BUILD_PACKAGING OFF)
option(WIL_BUILD_TESTS OFF)
FetchContent_MakeAvailable(wil)

# D3D12 Memory Allocator.
message(STATUS "Fetching D3D12MemoryAllocator...")
set(_saved_CMAKE_MESSAGE_LOG_LEVEL ${CMAKE_MESSAGE_LOG_LEVEL})
set(CMAKE_MESSAGE_LOG_LEVEL NOTICE)
FetchContent_Declare(
    d3d12memalloc
    GIT_REPOSITORY https://github.com/GPUOpen-LibrariesAndSDKs/D3D12MemoryAllocator
)
FetchContent_MakeAvailable(d3d12memalloc)
set(CMAKE_MESSAGE_LOG_LEVEL ${_saved_CMAKE_MESSAGE_LOG_LEVEL})

# DirectX Agility SDK.
message(STATUS "Fetching DirectX Agility SDK...")
FetchContent_Declare(
    dxagilitysdk
    URL "https://globalcdn.nuget.org/packages/microsoft.direct3d.d3d12.1.610.4.nupkg"
    URL_HASH MD5=33bffd67d09a9008e49b35d4a8c4b8b5
)
FetchContent_MakeAvailable(dxagilitysdk)
set(DXAGILITYSDK_SOURCE_DIR
    ${FETCHCONTENT_BASE_DIR}/dxagilitysdk-src/build/native
    CACHE INTERNAL "DirectX Agility SDK Source Directory"
)
add_library(dxagilitysdk STATIC
    ${DXAGILITYSDK_SOURCE_DIR}/src/d3dx12/d3dx12_property_format_table.cpp
)
target_include_directories(dxagilitysdk PRIVATE
    ${DXAGILITYSDK_SOURCE_DIR}/include
    ${DXAGILITYSDK_SOURCE_DIR}/include/d3dx12
)
target_compile_definitions(dxagilitysdk PRIVATE WIN32)
target_compile_options(dxagilitysdk PRIVATE -w)

# DirectX Shader Compiler.
message(STATUS "Fetching DirectX Shader Compiler...")
FetchContent_Declare(
    dxcompiler
    URL "https://globalcdn.nuget.org/packages/microsoft.direct3d.dxc.1.7.2212.36.nupkg"
    URL_HASH MD5=27673ca6b8e250e63ee5e741aeec1be4
)
FetchContent_MakeAvailable(dxcompiler)
set(DXCOMPILER_SOURCE_DIR
    ${FETCHCONTENT_BASE_DIR}/dxcompiler-src/build/native
    CACHE INTERNAL "DirectX Shader Compiler Source Directory"
)
