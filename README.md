# `framebuffet`

`framebuffet` is an experimental real-time rendering project whose primary focus is to catch up with the latest and the greatest in DirectX 12 Ultimate, HLSL 2021, C++17/20, and the related toolchain such as PIX, Visual Studio, and newer CMake.

## Key principles

- Make the best possible use of PIX. This means labeling every GPU resource, outputting shader PDBs for source-level debugging, and "Edit & Continue" functionality. Also, mark every GPU event with PIX events so that we can correlate NVIDIA's performance counters to draws and dispatches.
- Ensure that "Edit & Continue" works great in Visual Studio for rapid iteration. For example, ImGui can generally be Edit & Continue'd.
- Learn from well-established libraries such as DirectXTK12 and replace their functionality as we figure out a better alternative.
- This project is intentionally not cross-platform friendly (for example, the Win32 APIs are not hidden or wrapped away). Supporting other platforms and graphics APIs is a non-goal.

## Building

To build and run the project, you need a Windows 10+ machine with a GPU that supports Shader Model 6.6 dynamic resources and ray tracing. Currently, only RTX3080 and RTX4060 mobile are confirmed to work.

Example:

```
cmake -S . -B build -G "Visual Studio 17 2022" -T host=x64 -A x64
```
