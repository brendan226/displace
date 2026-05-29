# Displace

A minimal display driver for Windows. From here you can pretty much do whatever you want. Only thing I wanted to do was figure out what was the bare minimum to get pixels to the screen without allocating everything under the sun.
## Memory footprint

| State | Working set |
|---|---|
| render loop (red pixels)| ~14 MB |
| Post-init | ~2 MB |

## Red pixels demo

First rendered output — a full screen clear to red via `vkCmdClearColorImage`, no shader pipeline required.

![Red pixels](screenshots/pixels.png)

## Runtime memory

Runtime for red pixels

![Runtime memory](screenshots/runtime.png)

Runtime for driver only

![Runtime memory](screenshots/runtime2.png)

## Platform

Windows only, Nvidia only by design. This is not a limitation — it is the point.