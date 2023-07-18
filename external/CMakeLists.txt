include(FetchContent)

# Windows Implementation Library (WIL).
FetchContent_Declare(
    wil
    GIT_REPOSITORY https://github.com/microsoft/wil
)
option(WIL_BUILD_PACKAGING OFF)
option(WIL_BUILD_TESTS OFF)
FetchContent_MakeAvailable(wil)

# D3D12 Memory Allocator.
FetchContent_Declare(
    d3d12memalloc
    GIT_REPOSITORY https://github.com/GPUOpen-LibrariesAndSDKs/D3D12MemoryAllocator
)
FetchContent_MakeAvailable(d3d12memalloc)

# DirectX Agility SDK.
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
