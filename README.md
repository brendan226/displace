# Displace

A minimal Vulkan display driver for Windows. No frameworks, no bloat — just a raw Win32 window, a manually loaded Vulkan ICD, and direct GPU access.

## What it is

S2 is a bare-metal Vulkan display layer designed as the foundation for a game engine that uses Vulkan for presentation and CUDA for rendering and GPU communication. It exists because Nvidia doesn't expose the same low-level driver access AMD does — this bridges that gap using CUDA as the access layer Nvidia walls off.

## Design goals

- No dynamic linking overhead beyond what Windows forces on you
- No validation layers, no debug utils, no implicit extensions
- Manual function pointer loading via `GetProcAddress` — zero implicit Vulkan linkage
- Custom allocator hooked into every Vulkan call
- Stack allocation everywhere possible, no heap fragmentation
- Working set trimmed post-init so loader pages don't sit resident during the render loop

## Memory footprint

| State | Working set |
|---|---|
| Cold load, no trim | ~67 MB |
| Steady state render loop | ~14 MB |
| Post-init trim (`EmptyWorkingSet`) | ~14 MB |

The 67 MB spike is almost entirely the Nvidia ICD loading. After init pages are evicted, the steady state render cost is ~14 MB — the true runtime cost of a Vulkan window actively presenting frames on Windows with Nvidia.

## Red pixels demo

First rendered output — a full screen clear to red via `vkCmdClearColorImage`, no shader pipeline required.

![Red pixels](screenshots/pixels.png)

## Runtime memory

Task manager showing the 14 MB working set during active rendering after post-init trim.

![Runtime memory](screenshots/runtime.png)

## Usage

```c
// create your window first
HWND hwnd = CreateWindowEx(...);

// hand it to the driver
s2_load_vulkan(hwnd, hInstance);

// display context is now ready
// display.device, display.swapchain, display.queues etc
```

## Build

```bash
gcc -Ivulkan/Include -o main.exe *.c -Lvulkan/Lib -lgdi32 -luser32 -ldwmapi -lpsapi
```

## Platform

Windows only, Nvidia only by design. This is not a limitation — it is the point.