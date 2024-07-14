include(FetchContent)

# Note: Start by fetching and exposing external dependencies. This takes a long
# time to run, so we want to reuse the results in our subsequent runs.
if(NOT FETCH_CONTENT_HAS_BEEN_RUN)
    # Declarations.
    FetchContent_Declare(dearimgui GIT_REPOSITORY "https://github.com/ocornut/imgui" GIT_TAG cb16be3a3fc1f9cd146ae24d52b615f8a05fa93d) # v1.90.9
    FetchContent_Declare(cgltf GIT_REPOSITORY "https://github.com/jkuhlmann/cgltf" GIT_TAG 52c23814dbb60c2fbb54750ddf41d342d432a498) # v1.14
    FetchContent_Declare(stb GIT_REPOSITORY "https://github.com/nothings/stb" GIT_TAG 013ac3beddff3dbffafd5177e7972067cd2b5083) # May 31, 2024
    FetchContent_Declare(tinyexr GIT_REPOSITORY "https://github.com/syoyo/tinyexr" GIT_TAG 6c8742cc8145c8f629698cd8248900990946d6b1) # v1.0.8
    FetchContent_Declare(fp16 GIT_REPOSITORY "https://github.com/Maratyszcza/FP16" GIT_TAG 98b0a46bce017382a6351a19577ec43a715b6835) # Jun 20, 2024
    FetchContent_Declare(xxhash GIT_REPOSITORY "https://github.com/Cyan4973/xxHash" GIT_TAG bbb27a5efb85b92a0486cf361a8635715a53f6ba) # v0.8.2
    FetchContent_Declare(mikktspace GIT_REPOSITORY "https://github.com/mmikk/MikkTSpace" GIT_TAG 3e895b49d05ea07e4c2133156cfa94369e19e409) # Mar 25, 2020
    FetchContent_Declare(nlohmann_json GIT_REPOSITORY "https://github.com/nlohmann/json" GIT_TAG 9cca280a4d0ccf0c08f47a99aa71d1b0e52f8d03) # v3.11.3
    FetchContent_Declare(d3d12memalloc GIT_REPOSITORY "https://github.com/GPUOpen-LibrariesAndSDKs/D3D12MemoryAllocator" GIT_TAG 2b7c0d4e31dac5d893722573b0b2654270f907df) # Jun 17, 2024
    FetchContent_Declare(ttf2mesh GIT_REPOSITORY "https://github.com/fetisov/ttf2mesh" GIT_TAG b767f3e1026c979c6f2d6099576f206702065234) # Nov 3, 2023
    FetchContent_Declare(tracy GIT_REPOSITORY "https://github.com/wolfpld/tracy" GIT_TAG 37aff70dfa50cf6307b3fee6074d627dc2929143) # v0.10
    FetchContent_Declare(dxagilitysdk URL "https://globalcdn.nuget.org/packages/microsoft.direct3d.d3d12.1.614.0.nupkg" URL_MD5 65654a2bc55ed1afd8c3ea5b06d34fb1)
    FetchContent_Declare(dxcompiler URL "https://globalcdn.nuget.org/packages/microsoft.direct3d.dxc.1.8.2405.17.nupkg" URL_MD5 c9586b50602b558af84d3aacb43bfc2e)
    FetchContent_Declare(directxmath URL "https://globalcdn.nuget.org/packages/directxmath.2024.2.15.1.nupkg" URL_MD5 c9e372349ee8958b3599e88b89756715)
    FetchContent_Declare(directxtk12 URL "https://globalcdn.nuget.org/packages/directxtk12_desktop_2019.2024.6.5.1.nupkg" URL_MD5 d4b684f6b92c5f2e3a5a4c58a7729c6a)
    FetchContent_Declare(directxmesh URL "https://globalcdn.nuget.org/packages/directxmesh_desktop_win10.2024.6.5.1.nupkg" URL_MD5 c39e9ce826d390a9b42ca6fa12a889bb)
    FetchContent_Declare(directxtex URL "https://globalcdn.nuget.org/packages/directxtex_desktop_win10.2024.6.5.1.nupkg" URL_MD5 f580390887793e95a4876fd64e587a8b)
    FetchContent_Declare(winpixeventruntime URL "https://globalcdn.nuget.org/packages/winpixeventruntime.1.0.240308001.nupkg" URL_MD5 f376b719c0a7d84dd642ea7830b713f8)

    # Fetch.
    message(STATUS "Fetching Dear ImGui...")
    FetchContent_MakeAvailable(dearimgui)
    set(DEARIMGUI_SOURCE_DIR ${dearimgui_SOURCE_DIR} CACHE INTERNAL "Dear ImGui Source Directory")

    message(STATUS "Fetching cgltf...")
    FetchContent_MakeAvailable(cgltf)
    set(CGLTF_SOURCE_DIR ${cgltf_SOURCE_DIR} CACHE INTERNAL "cgltf Source Directory")

    message(STATUS "Fetching stb...")
    FetchContent_MakeAvailable(stb)
    set(STB_SOURCE_DIR ${stb_SOURCE_DIR} CACHE INTERNAL "stb Source Directory")

    message(STATUS "Fetching tinyexr...")
    FetchContent_GetProperties(tinyexr)

    if(NOT tinyexr_POPULATED)
        FetchContent_Populate(tinyexr)
        set(TINYEXR_SOURCE_DIR ${tinyexr_SOURCE_DIR} CACHE INTERNAL "tinyexr Source Directory")
    endif()

    message(STATUS "Fetching fp16...")
    FetchContent_GetProperties(fp16)

    if(NOT fp16_POPULATED)
        FetchContent_Populate(fp16)
        set(FP16_SOURCE_DIR ${fp16_SOURCE_DIR} CACHE INTERNAL "fp16 Source Directory")
    endif()

    message(STATUS "Fetching xxHash...")
    FetchContent_GetProperties(xxhash)

    if(NOT xxhash_POPULATED)
        FetchContent_Populate(xxhash)
        set(XXHASH_SOURCE_DIR ${xxhash_SOURCE_DIR} CACHE INTERNAL "xxHash Source Directory")
    endif()

    message(STATUS "Fetching mikktspace...")
    FetchContent_GetProperties(mikktspace)

    if(NOT mikktspace_POPULATED)
        FetchContent_Populate(mikktspace)
        set(MIKKTSPACE_SOURCE_DIR ${mikktspace_SOURCE_DIR} CACHE INTERNAL "mikktspace Source Directory")
    endif()

    FetchContent_GetProperties(nlohmann_json)

    if(NOT nlohmann_json_POPULATED)
        FetchContent_Populate(nlohmann_json)
        set(NLOHMANN_JSON_SOURCE_DIR ${nlohmann_json_SOURCE_DIR} CACHE INTERNAL "nlohmann_json Source Directory")
    endif()

    message(STATUS "Fetching D3D12MemoryAllocator...")
    FetchContent_GetProperties(d3d12memalloc)

    if(NOT d3d12memalloc_POPULATED)
        FetchContent_Populate(d3d12memalloc)
        set(D3D12MEMALLOC_SOURCE_DIR ${d3d12memalloc_SOURCE_DIR} CACHE INTERNAL "D3D12 Memory Allocator Source Directory")
    endif()

    message(STATUS "Fetching ttf2mesh...")
    FetchContent_MakeAvailable(ttf2mesh)
    set(TTF2MESH_SOURCE_DIR ${ttf2mesh_SOURCE_DIR} CACHE INTERNAL "ttf2mesh Source Directory")

    message(STATUS "Fetching Tracy...")
    FetchContent_MakeAvailable(tracy)
    set(TRACY_SOURCE_DIR ${tracy_SOURCE_DIR} CACHE INTERNAL "Tracy Source Directory")

    message(STATUS "Fetching DirectX Agility SDK...")
    FetchContent_MakeAvailable(dxagilitysdk)
    set(DXAGILITYSDK_SOURCE_DIR ${FETCHCONTENT_BASE_DIR}/dxagilitysdk-src/build/native CACHE INTERNAL "DirectX Agility SDK Source Directory")

    message(STATUS "Fetching DirectX Shader Compiler...")
    FetchContent_MakeAvailable(dxcompiler)
    set(DXCOMPILER_SOURCE_DIR ${FETCHCONTENT_BASE_DIR}/dxcompiler-src/build/native CACHE INTERNAL "DirectX Shader Compiler Source Directory")

    message(STATUS "Fetching DirectXMath...")
    FetchContent_MakeAvailable(directxmath)
    set(DIRECTXMATH_SOURCE_DIR ${FETCHCONTENT_BASE_DIR}/directxmath-src CACHE INTERNAL "DirectXMath Source Directory")

    message(STATUS "Fetching DirectXTK12...")
    FetchContent_MakeAvailable(directxtk12)
    set(DIRECTXTK12_SOURCE_DIR ${FETCHCONTENT_BASE_DIR}/directxtk12-src CACHE INTERNAL "DirectXTK12 Source Directory")

    message(STATUS "Fetching DirectXMesh...")
    FetchContent_MakeAvailable(directxmesh)
    set(DIRECTXMESH_SOURCE_DIR ${FETCHCONTENT_BASE_DIR}/directxmesh-src CACHE INTERNAL "DirectXMesh Source Directory")

    message(STATUS "Fetching DirectXTex...")
    FetchContent_MakeAvailable(directxtex)
    set(DIRECTXTEX_SOURCE_DIR ${FETCHCONTENT_BASE_DIR}/directxtex-src CACHE INTERNAL "DirectXTex Source Directory")

    message(STATUS "Fetching WinPixEventRuntime...")
    FetchContent_MakeAvailable(winpixeventruntime)
    set(WINPIXEVENTRUNTIME_SOURCE_DIR ${FETCHCONTENT_BASE_DIR}/winpixeventruntime-src CACHE INTERNAL "WinPixEventRuntime Source Directory")
endif()

# Note: Now every dependency is guaranteed to be available, proceed with
# configuring them.

# Dear ImGui.
add_library(
    ext_dearimgui STATIC
    ${DEARIMGUI_SOURCE_DIR}/imgui.cpp
    ${DEARIMGUI_SOURCE_DIR}/imgui_demo.cpp
    ${DEARIMGUI_SOURCE_DIR}/imgui_draw.cpp
    ${DEARIMGUI_SOURCE_DIR}/imgui_widgets.cpp
    ${DEARIMGUI_SOURCE_DIR}/imgui_tables.cpp
    ${DEARIMGUI_SOURCE_DIR}/backends/imgui_impl_win32.cpp
)
target_include_directories(
    ext_dearimgui
    PUBLIC
    ${DEARIMGUI_SOURCE_DIR}
    PRIVATE
    ${DEARIMGUI_SOURCE_DIR}/backends
)
file(GLOB_RECURSE DEARIMGUI_INCLUDE_FILES CONFIGURE_DEPENDS ${DEARIMGUI_SOURCE_DIR}/*.h ${DEARIMGUI_SOURCE_DIR}/*.inl)
target_sources(ext_dearimgui PRIVATE ${DEARIMGUI_INCLUDE_FILES})
set(DEARIMGUI_INCLUDE_DIR ${DEARIMGUI_SOURCE_DIR} CACHE INTERNAL "Dear ImGui Include Directory")
set(DEARIMGUI_LIBRARY ext_dearimgui CACHE INTERNAL "Dear ImGui Library")

# cgltf.
set(CGLTF_INCLUDE_DIR ${CGLTF_SOURCE_DIR} CACHE INTERNAL "cgltf Include Directory")

# stb.
set(STB_INCLUDE_DIR ${STB_SOURCE_DIR} CACHE INTERNAL "stb Include Directory")

# tinyexr.
add_library(
    ext_tinyexr STATIC
    ${TINYEXR_SOURCE_DIR}/tinyexr.cc
    ${TINYEXR_SOURCE_DIR}/deps/miniz/miniz.c
)
target_include_directories(
    ext_tinyexr
    PUBLIC
    ${TINYEXR_SOURCE_DIR}
    ${TINYEXR_SOURCE_DIR}/deps/miniz
)
set(TINYEXR_LIBRARY ext_tinyexr CACHE INTERNAL "tinyexr Library")

# fp16.
add_library(ext_fp16 INTERFACE)
target_include_directories(
    ext_fp16
    INTERFACE
    ${FP16_SOURCE_DIR}/include
)
set(FP16_LIBRARY ext_fp16 CACHE INTERNAL "fp16 Library")

# xxHash.
add_library(
    ext_xxhash STATIC
    ${XXHASH_SOURCE_DIR}/xxhash.c
)
target_include_directories(
    ext_xxhash
    PUBLIC
    ${XXHASH_SOURCE_DIR}
)
set(XXHASH_LIBRARY ext_xxhash CACHE INTERNAL "xxhash Library")

# mikktspace.
add_library(
    ext_mikktspace STATIC
    ${MIKKTSPACE_SOURCE_DIR}/mikktspace.c
)
target_include_directories(
    ext_mikktspace
    PUBLIC
    ${MIKKTSPACE_SOURCE_DIR}
)
set(MIKKTSPACE_LIBRARY ext_mikktspace CACHE INTERNAL "mikktspace Library")

# nlohmann_json.
add_library(ext_nlohmann_json INTERFACE)
target_include_directories(
    ext_nlohmann_json
    INTERFACE
    ${NLOHMANN_JSON_SOURCE_DIR}/single_include
)
target_compile_definitions(
    ext_nlohmann_json
    INTERFACE
    JSON_NO_IO
    JSON_NOEXCEPTION
    JSON_USE_IMPLICIT_CONVERSIONS=0
)
set(NLOHMANN_JSON_LIBRARY ext_nlohmann_json CACHE INTERNAL "nlohmann_json Library")

# ttf2mesh.
add_library(
    ext_ttf2mesh STATIC
    ${TTF2MESH_SOURCE_DIR}/ttf2mesh.c
    ${TTF2MESH_SOURCE_DIR}/ttf2mesh.h
)
target_include_directories(
    ext_ttf2mesh
    PUBLIC
    ${TTF2MESH_SOURCE_DIR}
)
set(TTF2MESH_LIBRARY ext_ttf2mesh CACHE INTERNAL "ttf2mesh Library")

# Tracy.
add_library(
    ext_tracy STATIC
    ${TRACY_SOURCE_DIR}/public/TracyClient.cpp
)
target_include_directories(
    ext_tracy
    PUBLIC
    ${TRACY_SOURCE_DIR}/public
)
target_compile_definitions(
    ext_tracy
    PUBLIC
    TRACY_ENABLE
    TRACY_ON_DEMAND
    TRACY_NO_BROADCAST
    TRACY_ONLY_LOCALHOST
)
set(TRACY_INCLUDE_DIR ${TRACY_SOURCE_DIR}/public CACHE INTERNAL "Tracy Include Directory")
set(TRACY_LIBRARY ext_tracy CACHE INTERNAL "Tracy Library")

# D3D12 Memory Allocator.
add_library(
    ext_d3d12memalloc STATIC
    ${D3D12MEMALLOC_SOURCE_DIR}/src/D3D12MemAlloc.cpp
)
target_include_directories(
    ext_d3d12memalloc
    PUBLIC
    ${D3D12MEMALLOC_SOURCE_DIR}/include
)
set(D3D12MEMALLOC_INCLUDE_DIR ${D3D12MemoryAllocator_SOURCE_DIR}/include CACHE INTERNAL "D3D12 Memory Allocator Include Directory")

# DirectX Agility SDK.
set(DXAGILITYSDK_INCLUDE_DIR ${DXAGILITYSDK_SOURCE_DIR}/include CACHE INTERNAL "DirectX Agility SDK Include Directory")
add_library(ext_dxagilitysdk STATIC
    ${DXAGILITYSDK_SOURCE_DIR}/src/d3dx12/d3dx12_property_format_table.cpp
)
target_include_directories(
    ext_dxagilitysdk PUBLIC
    ${DXAGILITYSDK_SOURCE_DIR}/include
    ${DXAGILITYSDK_SOURCE_DIR}/include/d3dx12
)
file(GLOB_RECURSE DXAGILITYSDK_INCLUDE_FILES CONFIGURE_DEPENDS ${DXAGILITYSDK_SOURCE_DIR}/include/*.h)
target_sources(ext_dxagilitysdk PRIVATE ${DXAGILITYSDK_INCLUDE_FILES})
target_compile_definitions(ext_dxagilitysdk PRIVATE WIN32)
set(DXAGILITYSDK_LIBRARY ext_dxagilitysdk CACHE INTERNAL "DirectX Agility SDK Library")

# DirectX Shader Compiler.
set(DXCOMPILER_INCLUDE_DIR ${DXCOMPILER_SOURCE_DIR}/include CACHE INTERNAL "DirectX Shader Compiler Include Directory")
set(DXCOMPILER_LINK_DIR ${DXCOMPILER_SOURCE_DIR}/lib/x64 CACHE INTERNAL "DirectX Shader Compiler Link Directory")

# DirectXMath.
set(DIRECTXMATH_INCLUDE_DIR ${DIRECTXMATH_SOURCE_DIR}/include CACHE INTERNAL "DirectXMath Include Directory")

# DirectXTK12.
file(GLOB_RECURSE DIRECTXTK12_INCLUDE_FILES ${DIRECTXTK12_SOURCE_DIR}/include/*.h ${DIRECTXTK12_SOURCE_DIR}/include/*.inl)
set(DIRECTXTK12_MODULE_INCLUDE_DIR ${DIRECTXTK12_SOURCE_DIR}/module-include/directxtk12)
file(MAKE_DIRECTORY ${DIRECTXTK12_MODULE_INCLUDE_DIR})
file(COPY ${DIRECTXTK12_INCLUDE_FILES} DESTINATION ${DIRECTXTK12_MODULE_INCLUDE_DIR})
set(DIRECTXTK12_SOURCE_DIR ${FETCHCONTENT_BASE_DIR}/directxtk12-src CACHE INTERNAL "DirectXTK12 Source Directory")
set(DIRECTXTK12_INCLUDE_DIR ${DIRECTXTK12_SOURCE_DIR}/module-include/ CACHE INTERNAL "DirectXTK12 Include Directory")
set(DIRECTXTK12_DEBUG_LINK_DIR ${DIRECTXTK12_SOURCE_DIR}/native/lib/x64/Debug CACHE INTERNAL "DirectXTK12 Debug Link Directory")
set(DIRECTXTK12_RELEASE_LINK_DIR ${DIRECTXTK12_SOURCE_DIR}/native/lib/x64/Release CACHE INTERNAL "DirectXTK12 Release Link Directory")
set(DIRECTXTK12_LIBRARY
    $<$<CONFIG:Debug>:${DIRECTXTK12_DEBUG_LINK_DIR}/DirectXTK12.lib>
    $<$<CONFIG:Release>:${DIRECTXTK12_RELEASE_LINK_DIR}/DirectXTK12.lib>
    CACHE INTERNAL "DirectX Agility SDK Library")

# DirectXMesh.
file(GLOB_RECURSE DIRECTXMESH_INCLUDE_FILES ${DIRECTXMESH_SOURCE_DIR}/include/*.h ${DIRECTXMESH_SOURCE_DIR}/include/*.inl)
set(DIRECTXMESH_MODULE_INCLUDE_DIR ${DIRECTXMESH_SOURCE_DIR}/module-include/directxmesh)
file(MAKE_DIRECTORY ${DIRECTXMESH_MODULE_INCLUDE_DIR})
file(COPY ${DIRECTXMESH_INCLUDE_FILES} DESTINATION ${DIRECTXMESH_MODULE_INCLUDE_DIR})
set(DIRECTXMESH_SOURCE_DIR ${FETCHCONTENT_BASE_DIR}/directxmesh-src CACHE INTERNAL "DirectXMesh Source Directory")
set(DIRECTXMESH_INCLUDE_DIR ${DIRECTXMESH_SOURCE_DIR}/module-include/ CACHE INTERNAL "DirectXMesh Include Directory")
set(DIRECTXMESH_DEBUG_LINK_DIR ${DIRECTXMESH_SOURCE_DIR}/native/lib/x64/Debug CACHE INTERNAL "DirectXMesh Debug Link Directory")
set(DIRECTXMESH_RELEASE_LINK_DIR ${DIRECTXMESH_SOURCE_DIR}/native/lib/x64/Release CACHE INTERNAL "DirectXMesh Release Link Directory")

# DirectXTex.
file(GLOB_RECURSE DIRECTXTEX_INCLUDE_FILES ${DIRECTXTEX_SOURCE_DIR}/include/*.h ${DIRECTXTEX_SOURCE_DIR}/include/*.inl)
set(DIRECTXTEX_MODULE_INCLUDE_DIR ${DIRECTXTEX_SOURCE_DIR}/module-include/directxtex)
file(MAKE_DIRECTORY ${DIRECTXTEX_MODULE_INCLUDE_DIR})
file(COPY ${DIRECTXTEX_INCLUDE_FILES} DESTINATION ${DIRECTXTEX_MODULE_INCLUDE_DIR})
set(DIRECTXTEX_SOURCE_DIR ${FETCHCONTENT_BASE_DIR}/directxtex-src CACHE INTERNAL "DirectXTex Source Directory")
set(DIRECTXTEX_INCLUDE_DIR ${DIRECTXTEX_SOURCE_DIR}/module-include/ CACHE INTERNAL "DirectXTex Include Directory")
set(DIRECTXTEX_DEBUG_LINK_DIR ${DIRECTXTEX_SOURCE_DIR}/native/lib/x64/Debug CACHE INTERNAL "DirectXTex Debug Link Directory")
set(DIRECTXTEX_RELEASE_LINK_DIR ${DIRECTXTEX_SOURCE_DIR}/native/lib/x64/Release CACHE INTERNAL "DirectXTex Release Link Directory")

# WinPixEventRuntime.
set(WINPIXEVENTRUNTIME_INCLUDE_DIR ${WINPIXEVENTRUNTIME_SOURCE_DIR}/Include CACHE INTERNAL "WinPixEventRuntime Include Directory")
set(WINPIXEVENTRUNTIME_BIN_DIR ${WINPIXEVENTRUNTIME_SOURCE_DIR}/bin/x64 CACHE INTERNAL "WinPixEventRuntime Binary Directory")
set(WINPIXEVENTRUNTIME_LINK_DIR ${WINPIXEVENTRUNTIME_SOURCE_DIR}/bin/x64 CACHE INTERNAL "WinPixEventRuntime Link Directory")
set(WINPIXEVENTRUNTIME_LIBRARY ${WINPIXEVENTRUNTIME_LINK_DIR}/WinPixEventRuntime.lib CACHE INTERNAL "WinPixEventRuntime Library")

# Set a flag to indicate that FetchContent has been run, so that we can avoid
# running it again.
set(FETCH_CONTENT_HAS_BEEN_RUN ON CACHE INTERNAL "Fetch Content Has Been Run")
