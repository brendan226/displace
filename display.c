#include <windows.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <psapi.h>

#define VK_USE_PLATFORM_WIN32_KHR
#include <vulkan/vulkan.h>
#include <vulkan/vulkan_win32.h>

#define S2_VKLIB "vulkan-1.dll"
#define S2_MAX_SWAPCHAIN_IMAGES 4
#define S2_REJECT_MS_BLOAT(x) EmptyWorkingSet(x)
#define S2_PROGRAM(x) GetCurrentProcess(x)

extern PFN_vkCreateInstance s2_vkCreateInstance;
extern PFN_vkGetInstanceProcAddr s2_vkGetInstanceProcAddr;

extern PFN_vkCreateWin32SurfaceKHR s2_vkCreateWin32SurfaceKHR;
extern PFN_vkEnumeratePhysicalDevices s2_vkEnumeratePhysicalDevices;
extern PFN_vkGetPhysicalDeviceQueueFamilyProperties s2_vkGetPhysicalDeviceQueueFamilyProperties;
extern PFN_vkGetPhysicalDeviceSurfaceSupportKHR s2_vkGetPhysicalDeviceSurfaceSupportKHR;
extern PFN_vkCreateDevice s2_vkCreateDevice;
extern PFN_vkGetPhysicalDeviceFormatProperties s2_vkGetPhysicalDeviceFormatProperties;
extern PFN_vkGetPhysicalDeviceSurfaceCapabilitiesKHR s2_vkGetPhysicalDeviceSurfaceCapabilitiesKHR;

extern PFN_vkGetDeviceQueue s2_vkGetDeviceQueue;
extern PFN_vkCreateSwapchainKHR s2_vkCreateSwapchainKHR;
extern PFN_vkGetSwapchainImagesKHR s2_vkGetSwapchainImagesKHR;
extern PFN_vkCreateImageView s2_vkCreateImageView;

typedef VkResult (VKAPI_PTR *PFN_vkCreateInstance)(const VkInstanceCreateInfo*, const VkAllocationCallbacks*, VkInstance*);
typedef PFN_vkVoidFunction (VKAPI_PTR *PFN_vkGetInstanceProcAddr)(VkInstance, const char*);
typedef PFN_vkVoidFunction (VKAPI_PTR *PFN_vkGetDeviceProcAddr)(VkDevice, const char*);
typedef VkResult (VKAPI_PTR *PFN_vkCreateWin32SurfaceKHR)(VkInstance, const VkWin32SurfaceCreateInfoKHR*, const VkAllocationCallbacks*, VkSurfaceKHR*);
typedef VkResult (VKAPI_PTR *PFN_vkEnumeratePhysicalDevices)(VkInstance, uint32_t*, VkPhysicalDevice*);
typedef void (VKAPI_PTR *PFN_vkGetPhysicalDeviceQueueFamilyProperties)(VkPhysicalDevice, uint32_t*, VkQueueFamilyProperties*);
typedef VkResult (VKAPI_PTR *PFN_vkGetPhysicalDeviceSurfaceSupportKHR)(VkPhysicalDevice, uint32_t, VkSurfaceKHR, VkBool32*);
typedef VkResult (VKAPI_PTR *PFN_vkCreateDevice)(VkPhysicalDevice, const VkDeviceCreateInfo*, const VkAllocationCallbacks*, VkDevice*);
typedef void (VKAPI_PTR *PFN_vkGetDeviceQueue)(VkDevice, uint32_t, uint32_t, VkQueue*);
typedef void (VKAPI_PTR *PFN_vkGetPhysicalDeviceFormatProperties)(VkPhysicalDevice, VkFormat, VkFormatProperties*);
typedef VkResult (VKAPI_PTR *PFN_vkGetPhysicalDeviceSurfaceCapabilitiesKHR)(VkPhysicalDevice, VkSurfaceKHR, VkSurfaceCapabilitiesKHR*);
typedef VkResult (VKAPI_PTR *PFN_vkCreateSwapchainKHR)(VkDevice, const VkSwapchainCreateInfoKHR*, const VkAllocationCallbacks*, VkSwapchainKHR*);
typedef VkResult (VKAPI_PTR *PFN_vkGetSwapchainImagesKHR)(VkDevice, VkSwapchainKHR, uint32_t*, VkImage*);
typedef VkResult (VKAPI_PTR *PFN_vkCreateImageView)(VkDevice, const VkImageViewCreateInfo*, const VkAllocationCallbacks*, VkImageView*);

#define S2_VKLIB "vulkan-1.dll"
#define S2_MAX_SWAPCHAIN_IMAGES 4

typedef struct {
    uint32_t graphics_family;
    uint32_t present_family;
    bool     graphics_found;
    bool     present_found;
} s2_queue_family;

typedef struct {
    VkInstance instance;
    VkPhysicalDevice physical_device;
    VkDevice device;

    VkSurfaceKHR surface;
    VkSwapchainKHR swapchain;

    VkQueue graphics_queue;
    VkQueue present_queue;

    VkImage images[S2_MAX_SWAPCHAIN_IMAGES];
    VkImageView image_views[S2_MAX_SWAPCHAIN_IMAGES];
    uint32_t image_count;

    VkFormat format;
    VkExtent2D extent;

    s2_queue_family queues;
} s2_display_context;

PFN_vkCreateInstance s2_vkCreateInstance = NULL;
PFN_vkGetInstanceProcAddr s2_vkGetInstanceProcAddr = NULL;
PFN_vkCreateWin32SurfaceKHR s2_vkCreateWin32SurfaceKHR = NULL;
PFN_vkEnumeratePhysicalDevices s2_vkEnumeratePhysicalDevices = NULL;
PFN_vkGetPhysicalDeviceQueueFamilyProperties s2_vkGetPhysicalDeviceQueueFamilyProperties = NULL;
PFN_vkGetPhysicalDeviceSurfaceSupportKHR s2_vkGetPhysicalDeviceSurfaceSupportKHR = NULL;
PFN_vkCreateDevice s2_vkCreateDevice = NULL;
PFN_vkGetPhysicalDeviceFormatProperties s2_vkGetPhysicalDeviceFormatProperties = NULL;
PFN_vkGetPhysicalDeviceSurfaceCapabilitiesKHR s2_vkGetPhysicalDeviceSurfaceCapabilitiesKHR = NULL;
PFN_vkGetDeviceQueue s2_vkGetDeviceQueue = NULL;
PFN_vkCreateSwapchainKHR s2_vkCreateSwapchainKHR = NULL;
PFN_vkGetSwapchainImagesKHR s2_vkGetSwapchainImagesKHR = NULL;
PFN_vkCreateImageView s2_vkCreateImageView = NULL;

// for example
PFN_vkCreateCommandPool     s2_vkCreateCommandPool     = NULL;
PFN_vkAllocateCommandBuffers s2_vkAllocateCommandBuffers = NULL;
PFN_vkBeginCommandBuffer    s2_vkBeginCommandBuffer    = NULL;
PFN_vkCmdPipelineBarrier    s2_vkCmdPipelineBarrier    = NULL;
PFN_vkCmdClearColorImage    s2_vkCmdClearColorImage    = NULL;
PFN_vkEndCommandBuffer      s2_vkEndCommandBuffer      = NULL;
PFN_vkCreateSemaphore       s2_vkCreateSemaphore       = NULL;
PFN_vkCreateFence           s2_vkCreateFence           = NULL;
PFN_vkAcquireNextImageKHR   s2_vkAcquireNextImageKHR   = NULL;
PFN_vkQueueSubmit           s2_vkQueueSubmit           = NULL;
PFN_vkQueuePresentKHR       s2_vkQueuePresentKHR       = NULL;
PFN_vkWaitForFences         s2_vkWaitForFences         = NULL;
PFN_vkResetFences           s2_vkResetFences           = NULL;
PFN_vkResetCommandBuffer    s2_vkResetCommandBuffer    = NULL;

s2_display_context display = {0};

static const char *s2_device_extensions[] = {
    VK_KHR_SWAPCHAIN_EXTENSION_NAME,
     VK_KHR_EXTERNAL_MEMORY_WIN32_EXTENSION_NAME
};

static void* VKAPI_PTR s2_vk_alloc(void* pUserData, size_t size, size_t alignment, VkSystemAllocationScope allocationScope)
{
    (void)pUserData; (void)allocationScope;
    return _aligned_malloc(size, alignment);
}

static void* VKAPI_PTR s2_vk_realloc(void* pUserData, void* pOriginal, size_t size, size_t alignment, VkSystemAllocationScope allocationScope)
{
    (void)pUserData; (void)allocationScope;
    return _aligned_realloc(pOriginal, size, alignment);
}

static void VKAPI_PTR s2_vk_free(void* pUserData, void* pMemory)
{
    (void)pUserData;
    _aligned_free(pMemory);
}

static const VkAllocationCallbacks s2_alloc_cb = {
    .pUserData = NULL,
    .pfnAllocation = s2_vk_alloc,
    .pfnReallocation = s2_vk_realloc,
    .pfnFree = s2_vk_free,
    .pfnInternalAllocation = NULL,
    .pfnInternalFree = NULL
};

static void s2_create_instance(VkInstance *instance)
{
    const char *ext[] = {
        VK_KHR_SURFACE_EXTENSION_NAME,
        VK_KHR_WIN32_SURFACE_EXTENSION_NAME,
        VK_KHR_EXTERNAL_MEMORY_CAPABILITIES_EXTENSION_NAME
    };
    
    VkApplicationInfo app = {};
    app.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    app.apiVersion = VK_API_VERSION_1_3;
    
    VkInstanceCreateInfo info = {};
    info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    info.pApplicationInfo = &app;
    info.enabledExtensionCount = 3;
    info.ppEnabledExtensionNames = ext;
    s2_vkCreateInstance(&info, &s2_alloc_cb, instance);
}

static VkExtent2D s2_choose_extent(VkSurfaceCapabilitiesKHR caps, HWND hwnd)
{
    if (caps.currentExtent.width != UINT32_MAX)
        return caps.currentExtent;

    RECT rect;
    GetClientRect(hwnd, &rect);

    VkExtent2D extent = {
        (uint32_t)(rect.right - rect.left),
        (uint32_t)(rect.bottom - rect.top)
    };

    if (extent.width < caps.minImageExtent.width)
        extent.width = caps.minImageExtent.width;

    if (extent.width > caps.maxImageExtent.width)
        extent.width = caps.maxImageExtent.width;

    if (extent.height < caps.minImageExtent.height)
        extent.height = caps.minImageExtent.height;

    if (extent.height > caps.maxImageExtent.height)
        extent.height = caps.maxImageExtent.height;

    return extent;
}

void s2_create_surface(VkInstance instance, HWND hwnd,
                              HINSTANCE hinstance, VkSurfaceKHR *surface)
{
    VkWin32SurfaceCreateInfoKHR info = {};
    info.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
    info.hwnd = hwnd;
    info.hinstance = hinstance;

    s2_vkCreateWin32SurfaceKHR(instance, &info, &s2_alloc_cb, surface);
}

s2_queue_family s2_find_queues(VkPhysicalDevice device, VkSurfaceKHR surface)
{
    s2_queue_family q = {0};
    uint32_t count = 0;
    s2_vkGetPhysicalDeviceQueueFamilyProperties(device, &count, NULL);
    
    // Stack allocation replaces dynamic malloc heap fragmentation
    VkQueueFamilyProperties props[8];
    if (count > 8) count = 8;
    s2_vkGetPhysicalDeviceQueueFamilyProperties(device, &count, props);
    
    for (uint32_t i = 0; i < count; i++) {
        if (props[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
            q.graphics_family = i;
            q.graphics_found  = true;
        }
        VkBool32 present = 0;
        s2_vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &present);
        if (present) {
            q.present_family = i;
            q.present_found  = true;
        }
    }
    return q;
}

void s2_pick_device(VkInstance instance, VkSurfaceKHR surface,
                           VkPhysicalDevice *device, s2_queue_family *queues)
{
    uint32_t count = 4;
    VkPhysicalDevice devices[4];
    s2_vkEnumeratePhysicalDevices(instance, &count, devices);
    
    if (count > 0) {
        *device = devices[0];
        *queues = s2_find_queues(*device, surface);
    }
}

void s2_create_device(VkPhysicalDevice phys, s2_queue_family queues,
                             VkDevice *device, VkQueue *graphics, VkQueue *present)
{
    float priority = 1.0f;
    VkDeviceQueueCreateInfo q[2] = {0};

    q[0].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    q[0].queueFamilyIndex = queues.graphics_family;
    q[0].queueCount = 1;
    q[0].pQueuePriorities = &priority;

    uint32_t queue_count = 1;
    if (queues.graphics_family != queues.present_family) {
        q[1].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        q[1].queueFamilyIndex = queues.present_family;
        q[1].queueCount = 1;
        q[1].pQueuePriorities = &priority;
        queue_count = 2;
    }

    VkPhysicalDeviceVulkan13Features f13 = {};
    f13.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES;
    f13.synchronization2 = VK_FALSE;
    f13.dynamicRendering = VK_FALSE;

    VkDeviceCreateInfo info = {};
    info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    info.pNext = &f13;
    info.queueCreateInfoCount = queue_count;
    info.pQueueCreateInfos = q;
    info.enabledExtensionCount = 2;
    info.ppEnabledExtensionNames = s2_device_extensions;

    s2_vkCreateDevice(phys, &info, &s2_alloc_cb, device);
    s2_vkGetDeviceQueue(*device, queues.graphics_family, 0, graphics);
    s2_vkGetDeviceQueue(*device, queues.present_family, 0, present);
}

void s2_create_swapchain(s2_display_context *ctx, HWND hwnd)
{
    VkFormatProperties fmt_props;
    s2_vkGetPhysicalDeviceFormatProperties(ctx->physical_device,
                                        VK_FORMAT_B8G8R8A8_UNORM, &fmt_props);
    if (!(fmt_props.optimalTilingFeatures & VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT)) {
        fprintf(stderr, "S2: B8G8R8A8_UNORM does not support storage, falling back to R8G8B8A8_UNORM\n");
        ctx->format = VK_FORMAT_R8G8B8A8_UNORM;
    } else {
        ctx->format = VK_FORMAT_B8G8R8A8_UNORM;
    }
    
    VkSurfaceCapabilitiesKHR caps;
    s2_vkGetPhysicalDeviceSurfaceCapabilitiesKHR(ctx->physical_device, ctx->surface, &caps);
    ctx->extent = s2_choose_extent(caps, hwnd);

    VkSwapchainCreateInfoKHR info = {};
    info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    info.surface = ctx->surface;
    info.minImageCount = 2;
    info.imageFormat = ctx->format;
    info.imageColorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;
    info.imageExtent = ctx->extent;
    info.imageArrayLayers = 1;
    info.imageUsage = VK_IMAGE_USAGE_STORAGE_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;
    info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    info.preTransform = caps.currentTransform;
    info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    info.presentMode = VK_PRESENT_MODE_FIFO_KHR;
    info.clipped = VK_TRUE;

    s2_vkCreateSwapchainKHR(ctx->device, &info, &s2_alloc_cb, &ctx->swapchain);
    
    uint32_t count = 0;
    s2_vkGetSwapchainImagesKHR(ctx->device, ctx->swapchain, &count, NULL);

    // Limit image count to preallocated structural bounds
    ctx->image_count = (count > S2_MAX_SWAPCHAIN_IMAGES) ? S2_MAX_SWAPCHAIN_IMAGES : count;
    s2_vkGetSwapchainImagesKHR(ctx->device, ctx->swapchain, &ctx->image_count, ctx->images);
}

void s2_create_image_views(s2_display_context *ctx)
{
    for (uint32_t i = 0; i < ctx->image_count; i++) {
        VkImageViewCreateInfo info = {};
        info.sType    = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        info.image    = ctx->images[i];
        info.viewType = VK_IMAGE_VIEW_TYPE_2D;
        info.format   = ctx->format;
        info.subresourceRange.aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT;
        info.subresourceRange.baseMipLevel   = 0;
        info.subresourceRange.levelCount     = 1;
        info.subresourceRange.baseArrayLayer = 0;
        info.subresourceRange.layerCount     = 1;
        
        s2_vkCreateImageView(ctx->device, &info, &s2_alloc_cb, &ctx->image_views[i]);
    }   
}

void s2_load_vulkan(HWND hwnd, HINSTANCE hinstance)
{
    HMODULE vklib = LoadLibraryA(S2_VKLIB);
    if (!vklib) return;

    s2_vkGetInstanceProcAddr = (PFN_vkGetInstanceProcAddr)
        GetProcAddress(vklib, "vkGetInstanceProcAddr");

    s2_vkCreateInstance = (PFN_vkCreateInstance)
        s2_vkGetInstanceProcAddr(VK_NULL_HANDLE, "vkCreateInstance");

    s2_create_instance(&display.instance);

    s2_vkCreateWin32SurfaceKHR = (PFN_vkCreateWin32SurfaceKHR)
        s2_vkGetInstanceProcAddr(display.instance, "vkCreateWin32SurfaceKHR");
    s2_vkEnumeratePhysicalDevices = (PFN_vkEnumeratePhysicalDevices)
        s2_vkGetInstanceProcAddr(display.instance, "vkEnumeratePhysicalDevices");
    s2_vkGetPhysicalDeviceQueueFamilyProperties = (PFN_vkGetPhysicalDeviceQueueFamilyProperties)
        s2_vkGetInstanceProcAddr(display.instance, "vkGetPhysicalDeviceQueueFamilyProperties");
    s2_vkGetPhysicalDeviceSurfaceSupportKHR = (PFN_vkGetPhysicalDeviceSurfaceSupportKHR)
        s2_vkGetInstanceProcAddr(display.instance, "vkGetPhysicalDeviceSurfaceSupportKHR");
    s2_vkCreateDevice = (PFN_vkCreateDevice)
        s2_vkGetInstanceProcAddr(display.instance, "vkCreateDevice");
    s2_vkGetPhysicalDeviceFormatProperties = (PFN_vkGetPhysicalDeviceFormatProperties)
        s2_vkGetInstanceProcAddr(display.instance, "vkGetPhysicalDeviceFormatProperties");
    s2_vkGetPhysicalDeviceSurfaceCapabilitiesKHR = (PFN_vkGetPhysicalDeviceSurfaceCapabilitiesKHR)
        s2_vkGetInstanceProcAddr(display.instance, "vkGetPhysicalDeviceSurfaceCapabilitiesKHR");
    
    s2_vkGetDeviceQueue = (PFN_vkGetDeviceQueue)
        s2_vkGetInstanceProcAddr(display.instance, "vkGetDeviceQueue");
    s2_vkCreateSwapchainKHR = (PFN_vkCreateSwapchainKHR)
        s2_vkGetInstanceProcAddr(display.instance, "vkCreateSwapchainKHR");
    s2_vkGetSwapchainImagesKHR = (PFN_vkGetSwapchainImagesKHR)
        s2_vkGetInstanceProcAddr(display.instance, "vkGetSwapchainImagesKHR");
    s2_vkCreateImageView = (PFN_vkCreateImageView)
        s2_vkGetInstanceProcAddr(display.instance, "vkCreateImageView");

    s2_create_surface(display.instance, hwnd, hinstance, &display.surface);
    s2_pick_device(display.instance, display.surface, &display.physical_device, &display.queues);
    s2_create_device(display.physical_device, display.queues,
                     &display.device, &display.graphics_queue, &display.present_queue);

    /* for example */
    s2_vkCreateCommandPool = (PFN_vkCreateCommandPool)
        s2_vkGetInstanceProcAddr(display.instance, "vkCreateCommandPool");
    s2_vkAllocateCommandBuffers = (PFN_vkAllocateCommandBuffers)
        s2_vkGetInstanceProcAddr(display.instance, "vkAllocateCommandBuffers");
    s2_vkBeginCommandBuffer = (PFN_vkBeginCommandBuffer)
        s2_vkGetInstanceProcAddr(display.instance, "vkBeginCommandBuffer");
    s2_vkCmdPipelineBarrier = (PFN_vkCmdPipelineBarrier)
        s2_vkGetInstanceProcAddr(display.instance, "vkCmdPipelineBarrier");
    s2_vkCmdClearColorImage = (PFN_vkCmdClearColorImage)
        s2_vkGetInstanceProcAddr(display.instance, "vkCmdClearColorImage");
    s2_vkEndCommandBuffer = (PFN_vkEndCommandBuffer)
        s2_vkGetInstanceProcAddr(display.instance, "vkEndCommandBuffer");
    s2_vkCreateSemaphore = (PFN_vkCreateSemaphore)
        s2_vkGetInstanceProcAddr(display.instance, "vkCreateSemaphore");
    s2_vkCreateFence = (PFN_vkCreateFence)
        s2_vkGetInstanceProcAddr(display.instance, "vkCreateFence");
    s2_vkAcquireNextImageKHR = (PFN_vkAcquireNextImageKHR)
        s2_vkGetInstanceProcAddr(display.instance, "vkAcquireNextImageKHR");
    s2_vkQueueSubmit = (PFN_vkQueueSubmit)
        s2_vkGetInstanceProcAddr(display.instance, "vkQueueSubmit");
    s2_vkQueuePresentKHR = (PFN_vkQueuePresentKHR)
        s2_vkGetInstanceProcAddr(display.instance, "vkQueuePresentKHR");
    s2_vkWaitForFences = (PFN_vkWaitForFences)
        s2_vkGetInstanceProcAddr(display.instance, "vkWaitForFences");
    s2_vkResetFences = (PFN_vkResetFences)
        s2_vkGetInstanceProcAddr(display.instance, "vkResetFences");
    s2_vkResetCommandBuffer = (PFN_vkResetCommandBuffer)
        s2_vkGetInstanceProcAddr(display.instance, "vkResetCommandBuffer");
    
    s2_create_swapchain(&display, hwnd);
    s2_create_image_views(&display);

    /* fuck off windows :)*/
    S2_REJECT_MS_BLOAT(S2_PROGRAM());
}
