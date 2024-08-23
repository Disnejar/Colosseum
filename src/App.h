#pragma once

// Dear ImGui: standalone example application for Glfw + Vulkan

// Learn about Dear ImGui:
// - FAQ                  https://dearimgui.com/faq
// - Getting Started      https://dearimgui.com/getting-started
// - Documentation        https://dearimgui.com/docs (same as your local docs/ folder).
// - Introduction, links and more at the top of imgui.cpp

// Important note to the reader who wish to integrate imgui_impl_vulkan.cpp/.h in their own engine/app.
// - Common ImGui_ImplVulkan_XXX functions and structures are used to interface with imgui_impl_vulkan.cpp/.h.
//   You will use those if you want to use this rendering backend in your engine/app.
// - Helper ImGui_ImplVulkanH_XXX functions and structures are only used by this example (main.cpp) and by
//   the backend itself (imgui_impl_vulkan.cpp), but should PROBABLY NOT be used by your own engine/app code.
// Read comments in imgui_impl_vulkan.h.

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_vulkan.h"

#include <stdio.h>          // printf, fprintf
#include <stdlib.h>         // abort
#include <memory>
#include <vector>
#include <algorithm>
#include <functional>

#define GLFW_INCLUDE_NONE
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

// Volk headers
#ifdef IMGUI_IMPL_VULKAN_USE_VOLK
#define VOLK_IMPLEMENTATION
#include <volk.h>
#endif

#include "Layer.h"
#include "Utils.h"

// [Win32] Our example includes a copy of glfw3.lib pre-compiled with VS2010 to maximize ease of testing and compatibility with old VS compilers.
// To link with VS2010-era libraries, VS2015+ requires linking with legacy_stdio_definitions.lib, which we do using this pragma.
// Your own project should not be affected, as you are likely to link with a newer binary of GLFW that is adequate for your version of Visual Studio.
#if defined(_MSC_VER) && (_MSC_VER >= 1900) && !defined(IMGUI_DISABLE_WIN32_FUNCTIONS)
#pragma comment(lib, "legacy_stdio_definitions")
#endif

//#define APP_USE_UNLIMITED_FRAME_RATE
#ifdef _DEBUG
#define APP_USE_VULKAN_DEBUG_REPORT
#endif

namespace Colosseum {
    namespace App
    {
        // Data
        extern VkAllocationCallbacks*       g_Allocator;
        extern VkInstance                   g_Instance;
        extern VkPhysicalDevice             g_PhysicalDevice;
        extern VkDevice                     g_Device;
        extern uint32_t                     g_QueueFamily;
        extern VkQueue                      g_Queue;
        extern VkDebugReportCallbackEXT     g_DebugReport;
        extern VkPipelineCache              g_PipelineCache;
        extern VkDescriptorPool             g_DescriptorPool;
        extern ImGui_ImplVulkanH_Window     g_MainWindowData;
        extern int                          g_MinImageCount;
        extern bool                         g_SwapChainRebuild;

        extern GLFWwindow*                  g_glfwWindow;
        extern ImGui_ImplVulkanH_Window*    g_vulkanWindow;

        extern std::vector<std::vector<VkCommandBuffer>> g_AllocatedCommandBuffers;
        extern std::vector<std::vector<std::function<void()>>> g_ResourceFreeQueue;

        extern uint32_t g_CurrentFrameIndex;

        extern std::vector<std::shared_ptr<Layer>> g_LayerStack;

        bool IsExtensionAvailable(const ImVector<VkExtensionProperties>& properties, const char* extension);
        VkPhysicalDevice SetupVulkan_SelectPhysicalDevice();
        void SetupVulkan(ImVector<const char*> instance_extensions);
        // All the ImGui_ImplVulkanH_XXX structures/functions are optional helpers used by the demo.
        // Your real engine/app may not use them.
        void SetupVulkanWindow(ImGui_ImplVulkanH_Window* wd, VkSurfaceKHR surface, int width, int height);
        void CleanupVulkan();
        void CleanupVulkanWindow();
        void FrameRender(ImGui_ImplVulkanH_Window* wd, ImDrawData* draw_data);
        void FramePresent(ImGui_ImplVulkanH_Window* wd);

        int Initialise();
        int Run();
        int Cleanup();
        void AddLayer(std::shared_ptr<Layer> layer_ptr);
        void RemoveLayer(std::shared_ptr<Layer> layer_ptr);

        VkCommandBuffer GetCommandBuffer(bool begin);
        void FlushCommandBuffer(VkCommandBuffer commandBuffer);

        void SubmitResourceFree(std::function<void()> &&func);
    }
}