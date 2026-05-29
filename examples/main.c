#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <windows.h>
#include <dwmapi.h>
#include <psapi.h>

#include "display.h"

extern PFN_vkCreateCommandPool      s2_vkCreateCommandPool;
extern PFN_vkAllocateCommandBuffers s2_vkAllocateCommandBuffers;
extern PFN_vkBeginCommandBuffer     s2_vkBeginCommandBuffer;
extern PFN_vkCmdPipelineBarrier     s2_vkCmdPipelineBarrier;
extern PFN_vkCmdClearColorImage     s2_vkCmdClearColorImage;
extern PFN_vkEndCommandBuffer       s2_vkEndCommandBuffer;
extern PFN_vkCreateSemaphore        s2_vkCreateSemaphore;
extern PFN_vkCreateFence            s2_vkCreateFence;
extern PFN_vkAcquireNextImageKHR    s2_vkAcquireNextImageKHR;
extern PFN_vkQueueSubmit            s2_vkQueueSubmit;
extern PFN_vkQueuePresentKHR        s2_vkQueuePresentKHR;
extern PFN_vkWaitForFences          s2_vkWaitForFences;
extern PFN_vkResetFences            s2_vkResetFences;
extern PFN_vkResetCommandBuffer     s2_vkResetCommandBuffer;

static bool running = true;

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    PAINTSTRUCT ps;
    HDC hdc;

    switch (msg) {
    case WM_PAINT:
        hdc = BeginPaint(hwnd, &ps);
        (void)hdc;
        EndPaint(hwnd, &ps);
        return 0;

    case WM_SIZING: {
        RECT *r = (RECT*)lParam;
        int w = r->right - r->left;
        int h = r->bottom - r->top;
        h = (w * 9) / 16;
        r->bottom = r->top + h;
        return TRUE;
    }

    case WM_DESTROY:
        running = false;
        PostQuitMessage(0);
        return 0;

    case WM_KEYDOWN:
        if (wParam == VK_ESCAPE) {
            running = false;
            DestroyWindow(hwnd);
        }
        return 0;

    default:
        return DefWindowProc(hwnd, msg, wParam, lParam);
    }
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{   
        WNDCLASS wc = {0};
    HWND hwnd;
    MSG msg;
    const char *CLASS_NAME = "example";

    (void)hPrevInstance; (void)lpCmdLine; (void)nCmdShow;

    wc.style         = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc   = WndProc;
    wc.hInstance     = hInstance;
    wc.hbrBackground = NULL;
    wc.hCursor       = NULL;
    wc.lpszClassName = CLASS_NAME;
    
    if (!RegisterClass(&wc)) {
        MessageBox(NULL, "RegisterClass failed!", "Error", MB_ICONERROR);
        return 1;
    }

    hwnd = CreateWindowEx(0, CLASS_NAME, "Displace.exe", WS_OVERLAPPEDWINDOW,
                          CW_USEDEFAULT, CW_USEDEFAULT, 800, 600,
                          NULL, NULL, hInstance, NULL);
    if (!hwnd) {
        MessageBox(NULL, "S2 failed to create a window", "Error", MB_ICONERROR);
        return 1;
    }

    s2_load_vulkan(hwnd, hInstance);
    // cut
    VkCommandPool cmd_pool;
    VkCommandBuffer cmd_buf;
    VkSemaphore sem_acquire, sem_release;
    VkFence fence;

    // command pool
    VkCommandPoolCreateInfo pool_info = {0};
    pool_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    pool_info.queueFamilyIndex = display.queues.graphics_family;
    pool_info.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    s2_vkCreateCommandPool(display.device, &pool_info, NULL, &cmd_pool);

    // command buffer
    VkCommandBufferAllocateInfo alloc_info = {0};
    alloc_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    alloc_info.commandPool = cmd_pool;
    alloc_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    alloc_info.commandBufferCount = 1;
    s2_vkAllocateCommandBuffers(display.device, &alloc_info, &cmd_buf);

    // semaphores + fence
    VkSemaphoreCreateInfo sem_info = {0};
    sem_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
    s2_vkCreateSemaphore(display.device, &sem_info, NULL, &sem_acquire);
    s2_vkCreateSemaphore(display.device, &sem_info, NULL, &sem_release);

    VkFenceCreateInfo fence_info = {0};
    fence_info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fence_info.flags = VK_FENCE_CREATE_SIGNALED_BIT;
    s2_vkCreateFence(display.device, &fence_info, NULL, &fence);

    ShowWindow(hwnd, SW_SHOW);
    UpdateWindow(hwnd);

    while (running) {

        while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
            if (msg.message == WM_QUIT)
                running = false;

            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

        if (!running)
            break;
        
        s2_vkWaitForFences(display.device, 1, &fence, VK_TRUE, UINT64_MAX);
        s2_vkResetFences(display.device, 1, &fence);

        // acquire
        uint32_t img_index;
        s2_vkAcquireNextImageKHR(display.device, display.swapchain, UINT64_MAX,
                                 sem_acquire, VK_NULL_HANDLE, &img_index);

        // record
        s2_vkResetCommandBuffer(cmd_buf, 0);
        VkCommandBufferBeginInfo begin = {0};
        begin.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        begin.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
        s2_vkBeginCommandBuffer(cmd_buf, &begin);

        // transition: UNDEFINED → TRANSFER_DST
        VkImageMemoryBarrier barrier = {0};
        barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        barrier.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        barrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
        barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.image = display.images[img_index];
        barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        barrier.subresourceRange.levelCount = 1;
        barrier.subresourceRange.layerCount = 1;
        barrier.srcAccessMask = 0;
        barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        s2_vkCmdPipelineBarrier(cmd_buf,
                                VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT,
                                0, 0, NULL, 0, NULL, 1, &barrier);

        // clear to red
        VkClearColorValue color = {{1.0f, 0.0f, 0.0f, 1.0f}};
        VkImageSubresourceRange range = {0};
        range.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        range.levelCount = 1;
        range.layerCount = 1;
        s2_vkCmdClearColorImage(cmd_buf, display.images[img_index],
                                VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, &color, 1, &range);

        // transition: TRANSFER_DST → PRESENT_SRC
        barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
        barrier.newLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
        barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        barrier.dstAccessMask = 0;
        s2_vkCmdPipelineBarrier(cmd_buf,
                                VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT,
                                0, 0, NULL, 0, NULL, 1, &barrier);

        s2_vkEndCommandBuffer(cmd_buf);

        // submit
        VkPipelineStageFlags wait_stage = VK_PIPELINE_STAGE_TRANSFER_BIT;
        VkSubmitInfo submit = {0};
        submit.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submit.waitSemaphoreCount = 1;
        submit.pWaitSemaphores = &sem_acquire;
        submit.pWaitDstStageMask = &wait_stage;
        submit.commandBufferCount = 1;
        submit.pCommandBuffers = &cmd_buf;
        submit.signalSemaphoreCount = 1;
        submit.pSignalSemaphores = &sem_release;
        s2_vkQueueSubmit(display.graphics_queue, 1, &submit, fence);

        // present
        VkPresentInfoKHR present = {0};
        present.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
        present.waitSemaphoreCount = 1;
        present.pWaitSemaphores = &sem_release;
        present.swapchainCount = 1;
        present.pSwapchains = &display.swapchain;
        present.pImageIndices = &img_index;
        s2_vkQueuePresentKHR(display.present_queue, &present);
    }
        
    return (int)msg.wParam;
}
