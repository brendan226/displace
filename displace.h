#ifndef S2_DISPLACE_H
#define S2_DISPLACE_H

#include <windows.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <dwmapi.h>
#include <psapi.h>

#define VK_USE_PLATFORM_WIN32_KHR
#include <vulkan/vulkan.h>
#include <vulkan/vulkan_win32.h>

typedef struct {
    uint32_t graphics_family;
    uint32_t present_family;
    bool     graphics_found;
    bool     present_found;
} queue_family;

typedef struct {
    VkInstance instance;
    VkPhysicalDevice physical_device;
    VkDevice device;
    VkSurfaceKHR surface;
    VkSwapchainKHR swapchain;
    VkQueue graphics_queue;
    VkQueue present_queue;
    VkImage images[4];
    VkImageView image_views[4];
    uint32_t image_count;
    VkFormat format;
    VkExtent2D extent;
    queue_family queues;
} display_context;

extern display_context display;

void load_vulkan(HWND hwnd, HINSTANCE hinstance);

#endif /* DISPLAY_H */
