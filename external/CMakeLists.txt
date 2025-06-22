include(FetchContent)

# Declarations.
FetchContent_Declare(dearimgui GIT_REPOSITORY "https://github.com/ocornut/imgui" GIT_TAG f5befd2d29e66809cd1110a152e375a7f1981f06) # v1.91.9b
FetchContent_Declare(cgltf GIT_REPOSITORY "https://github.com/jkuhlmann/cgltf" GIT_TAG 360db1a95480fe102ae9c69b27c5d101167ff5ba) # v1.15
FetchContent_Declare(stb GIT_REPOSITORY "https://github.com/nothings/stb" GIT_TAG f58f558c120e9b32c217290b80bad1a0729fbb2c) # May 26, 2025
FetchContent_Declare(tinyexr GIT_REPOSITORY "https://github.com/syoyo/tinyexr" GIT_TAG 735ff73ce5959cf005eb99ce517c9bcecab89dfb) # v1.0.12
FetchContent_Declare(fp16 GIT_REPOSITORY "https://github.com/Maratyszcza/FP16" GIT_TAG 98b0a46bce017382a6351a19577ec43a715b6835) # Jun 20, 2024
FetchContent_Declare(xxhash GIT_REPOSITORY "https://github.com/Cyan4973/xxHash" GIT_TAG e626a72bc2321cd320e953a0ccf1584cad60f363) # v0.8.3
FetchContent_Declare(mikktspace GIT_REPOSITORY "https://github.com/mmikk/MikkTSpace" GIT_TAG 3e895b49d05ea07e4c2133156cfa94369e19e409) # Mar 25, 2020
FetchContent_Declare(nlohmann_json GIT_REPOSITORY "https://github.com/nlohmann/json" GIT_TAG 55f93686c01528224f448c19128836e7df245f72) # v3.12.0
FetchContent_Declare(d3d12memalloc GIT_REPOSITORY "https://github.com/GPUOpen-LibrariesAndSDKs/D3D12MemoryAllocator" GIT_TAG 9ef66bc14edd10dee0de3a545b98578363552f66) # v3.0.1
FetchContent_Declare(ttf2mesh GIT_REPOSITORY "https://github.com/fetisov/ttf2mesh" GIT_TAG b767f3e1026c979c6f2d6099576f206702065234) # Nov 3, 2023
FetchContent_Declare(tracy GIT_REPOSITORY "https://github.com/wolfpld/tracy" GIT_TAG 650c98ece70da9e155e7ba1c2b3ee16004aae118) # v0.12.1
FetchContent_Declare(glm GIT_REPOSITORY "https://github.com/g-truc/glm.git" GIT_TAG 0af55ccecd98d4e5a8d1fad7de25ba429d60e863) # v1.0.1
FetchContent_Declare(catch2 GIT_REPOSITORY "https://github.com/catchorg/Catch2/" GIT_TAG 2b60af89e23d28eefc081bc930831ee9d45ea58b) # v3.8.1
FetchContent_Declare(gettimesinceprocessstart GIT_REPOSITORY "https://github.com/maxliani/GetTimeSinceProcessStart" GIT_TAG b2fa1a71f1da91d3b009430a7c49f35f5fb8c7c5) # v1.0.0
FetchContent_Declare(dxagilitysdk URL "https://globalcdn.nuget.org/packages/microsoft.direct3d.d3d12.1.616.1.nupkg" URL_MD5 7c7aa4a5a4c71df7892e5fa815b6818c)
FetchContent_Declare(dxcompiler URL "https://globalcdn.nuget.org/packages/microsoft.direct3d.dxc.1.8.2505.28.nupkg" URL_MD5 edcbac8d991c33f8bc8001e8c9c98ada)
FetchContent_Declare(directxmath URL "https://globalcdn.nuget.org/packages/directxmath.2025.4.3.1.nupkg" URL_MD5 92ae9bd406510205959e62353cbcb610)
FetchContent_Declare(directxmesh URL "https://globalcdn.nuget.org/packages/directxmesh_desktop_win10.2025.3.25.2.nupkg" URL_MD5 3566f3a4de68dab6d5cd2becd969533a)
FetchContent_Declare(directxtex URL "https://globalcdn.nuget.org/packages/directxtex_desktop_win10.2025.3.25.2.nupkg" URL_MD5 80456c80b55a8b9da84cded77b7abaef)
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
FetchContent_MakeAvailable(tinyexr)
set(TINYEXR_SOURCE_DIR ${tinyexr_SOURCE_DIR} CACHE INTERNAL "tinyexr Source Directory")

message(STATUS "Fetching fp16...")
set(FP16_BUILD_TESTS OFF CACHE INTERNAL "")
set(FP16_BUILD_BENCHMARKS OFF CACHE INTERNAL "")
set(FP16_BUILD_COMPARATIVE_BENCHMARKS OFF CACHE INTERNAL "")
FetchContent_MakeAvailable(fp16)
set(FP16_SOURCE_DIR ${fp16_SOURCE_DIR} CACHE INTERNAL "fp16 Source Directory")

message(STATUS "Fetching xxHash...")
FetchContent_MakeAvailable(xxhash)
set(XXHASH_SOURCE_DIR ${xxhash_SOURCE_DIR} CACHE INTERNAL "xxHash Source Directory")

message(STATUS "Fetching mikktspace...")
FetchContent_MakeAvailable(mikktspace)
set(MIKKTSPACE_SOURCE_DIR ${mikktspace_SOURCE_DIR} CACHE INTERNAL "mikktspace Source Directory")

message(STATUS "Fetching nlohmann_json...")
FetchContent_MakeAvailable(nlohmann_json)
set(NLOHMANN_JSON_SOURCE_DIR ${nlohmann_json_SOURCE_DIR} CACHE INTERNAL "nlohmann_json Source Directory")

message(STATUS "Fetching D3D12MemoryAllocator...")
FetchContent_MakeAvailable(d3d12memalloc)
set(D3D12MEMALLOC_SOURCE_DIR ${d3d12memalloc_SOURCE_DIR} CACHE INTERNAL "D3D12 Memory Allocator Source Directory")

message(STATUS "Fetching ttf2mesh...")
FetchContent_MakeAvailable(ttf2mesh)
set(TTF2MESH_SOURCE_DIR ${ttf2mesh_SOURCE_DIR} CACHE INTERNAL "ttf2mesh Source Directory")

message(STATUS "Fetching Tracy...")
FetchContent_MakeAvailable(tracy)
set(TRACY_SOURCE_DIR ${tracy_SOURCE_DIR} CACHE INTERNAL "Tracy Source Directory")

message(STATUS "Fetching GLM...")
FetchContent_MakeAvailable(glm)
set(GLM_SOURCE_DIR ${glm_SOURCE_DIR} CACHE INTERNAL "GLM Source Directory")

message(STATUS "Fetching catch2...")
FetchContent_MakeAvailable(catch2)
set(CATCH2_SOURCE_DIR ${catch2_SOURCE_DIR} CACHE INTERNAL "CATCH2 Source Directory")

message(STATUS "Fetching GetTimeSinceProcessStart...")
FetchContent_MakeAvailable(gettimesinceprocessstart)
set(GETTIMESINCEPROCESSSTART_SOURCE_DIR ${gettimesinceprocessstart_SOURCE_DIR} CACHE INTERNAL "GetTimeSinceProcessStart Source Directory")

message(STATUS "Fetching DirectX Agility SDK...")
FetchContent_MakeAvailable(dxagilitysdk)
set(DXAGILITYSDK_SOURCE_DIR ${FETCHCONTENT_BASE_DIR}/dxagilitysdk-src/build/native CACHE INTERNAL "DirectX Agility SDK Source Directory")

message(STATUS "Fetching DirectX Shader Compiler...")
FetchContent_MakeAvailable(dxcompiler)
set(DXCOMPILER_SOURCE_DIR ${FETCHCONTENT_BASE_DIR}/dxcompiler-src/build/native CACHE INTERNAL "DirectX Shader Compiler Source Directory")

message(STATUS "Fetching DirectXMath...")
FetchContent_MakeAvailable(directxmath)
set(DIRECTXMATH_SOURCE_DIR ${FETCHCONTENT_BASE_DIR}/directxmath-src CACHE INTERNAL "DirectXMath Source Directory")

message(STATUS "Fetching DirectXMesh...")
FetchContent_MakeAvailable(directxmesh)
set(DIRECTXMESH_SOURCE_DIR ${FETCHCONTENT_BASE_DIR}/directxmesh-src CACHE INTERNAL "DirectXMesh Source Directory")

message(STATUS "Fetching DirectXTex...")
FetchContent_MakeAvailable(directxtex)
set(DIRECTXTEX_SOURCE_DIR ${FETCHCONTENT_BASE_DIR}/directxtex-src CACHE INTERNAL "DirectXTex Source Directory")

message(STATUS "Fetching WinPixEventRuntime...")
FetchContent_MakeAvailable(winpixeventruntime)
set(WINPIXEVENTRUNTIME_SOURCE_DIR ${FETCHCONTENT_BASE_DIR}/winpixeventruntime-src CACHE INTERNAL "WinPixEventRuntime Source Directory")

# Ignore clang-tidy for external dependencies.
configure_file(cmake/clang-tidy.in ${CMAKE_BINARY_DIR}/.clang-tidy COPYONLY)

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
configure_file(cmake/cgltf.cpp.in ${CGLTF_SOURCE_DIR}/cgltf.cpp COPYONLY)
add_library(ext_cgltf STATIC ${CGLTF_SOURCE_DIR}/cgltf.cpp)
target_include_directories(
    ext_cgltf
    PUBLIC
    ${CGLTF_SOURCE_DIR}
)
set(CGLTF_LIBRARY ext_cgltf CACHE INTERNAL "cgltf Library")
set(CGLTF_INCLUDE_DIR ${CGLTF_SOURCE_DIR} CACHE INTERNAL "cgltf Include Directory")

# stb.
configure_file(cmake/stb.cpp.in ${STB_SOURCE_DIR}/stb.cpp COPYONLY)
add_library(ext_stb STATIC ${STB_SOURCE_DIR}/stb.cpp)
target_include_directories(
    ext_stb
    PUBLIC
    ${STB_SOURCE_DIR}
)
set(STB_LIBRARY ext_stb CACHE INTERNAL "stb Library")
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
    TRACY_NO_SAMPLING
    TRACY_NO_CONTEXT_SWITCH
    TRACY_NO_FRAME_IMAGE
)
set(TRACY_INCLUDE_DIR ${TRACY_SOURCE_DIR}/public CACHE INTERNAL "Tracy Include Directory")
set(TRACY_LIBRARY ext_tracy CACHE INTERNAL "Tracy Library")

# glm.
add_library(ext_glm INTERFACE)
target_include_directories(
    ext_glm
    INTERFACE
    ${GLM_SOURCE_DIR}
)
target_compile_definitions(
    ext_glm
    INTERFACE
    GLM_FORCE_CXX20
    GLM_FORCE_INLINE
    GLM_FORCE_DEPTH_ZERO_TO_ONE
    GLM_FORCE_RIGHT_HANDED # Doesn't exist, just to communicate handedness.
    GLM_FORCE_EXPLICIT_CTOR
    GLM_FORCE_CTOR_INIT
    GLM_FORCE_XYZW_ONLY
    GLM_ENABLE_EXPERIMENTAL
)
set(GLM_INCLUDE_DIR ${GLM_SOURCE_DIR} CACHE INTERNAL "GLM Include Directory")
set(GLM_LIBRARY ext_glm CACHE INTERNAL "GLM Library")

# catch2.
add_library(ext_catch2 STATIC
    ${CATCH2_SOURCE_DIR}/extras/catch_amalgamated.cpp
)
target_include_directories(ext_catch2 PUBLIC ${CATCH2_SOURCE_DIR}/extras)
target_compile_definitions(ext_catch2 PUBLIC CATCH_AMALGAMATED_CUSTOM_MAIN)
set(CATCH2_LIBRARY ext_catch2 CACHE INTERNAL "Catch2 Library")

# GetTimeSinceProcessStart.
configure_file(cmake/gettimesinceprocessstart.cpp.in ${GETTIMESINCEPROCESSSTART_SOURCE_DIR}/GetTimeSinceProcessStart.cpp COPYONLY)
add_library(ext_gettimesinceprocessstart STATIC ${GETTIMESINCEPROCESSSTART_SOURCE_DIR}/GetTimeSinceProcessStart.cpp)
target_include_directories(
    ext_gettimesinceprocessstart
    PUBLIC
    ${GETTIMESINCEPROCESSSTART_SOURCE_DIR}
)
set(GETTIMESINCEPROCESSSTART_LIBRARY ext_gettimesinceprocessstart CACHE INTERNAL "GetTimeSinceProcessStart Library")
set(GETTIMESINCEPROCESSSTART_INCLUDE_DIR ${GETTIMESINCEPROCESSSTART_SOURCE_DIR} CACHE INTERNAL "GetTimeSinceProcessStart Include Directory")

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
