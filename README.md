# Displace

A minimal display driver for Windows. From here you can pretty much do whatever you want. Only thing I wanted to do was figure out what was the bare minimum to get pixels to the screen without allocating everything under the sun. Displace is a small Vulkan display layer for any windows appilcation that needs pixels on the screen without the bloat.  

## Memory footprint

| State |
|---|---|
| Post-init | ~2 MB |

## Red pixels demo

First rendered output — a full screen clear to red via `vkCmdClearColorImage`, no shader pipeline required.

![Red pixels](screenshots/pixels.png)

## Runtime memory

Runtime for red pixels

![Runtime memory](screenshots/runtime2.png)

Loading driver only

## Build

```bash
gcc -Ivulkan/Include -o main.exe *.c -Lvulkan/Lib -lgdi32 -luser32 -ldwmapi -lpsapi
```

## Platform

Windows only, Nvidia only by design. This is not a limitation — it is the point.