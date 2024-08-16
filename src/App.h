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
#define GLFW_INCLUDE_NONE
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

// Volk headers
#ifdef IMGUI_IMPL_VULKAN_USE_VOLK
#define VOLK_IMPLEMENTATION
#include <volk.h>
#endif

#include "Layer.h"

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
    class App
    {
    private:
        // Data
        static VkAllocationCallbacks*   g_Allocator;
        static VkInstance               g_Instance;
        static VkPhysicalDevice         g_PhysicalDevice;
        static VkDevice                 g_Device;
        static uint32_t                 g_QueueFamily;
        static VkQueue                  g_Queue;
        static VkDebugReportCallbackEXT g_DebugReport;
        static VkPipelineCache          g_PipelineCache;
        static VkDescriptorPool         g_DescriptorPool;
        static ImGui_ImplVulkanH_Window g_MainWindowData;
        static int                      g_MinImageCount;
        static bool                     g_SwapChainRebuild;

        static std::vector<std::shared_ptr<Layer>> _LayerStack;

    private:
        static void glfw_error_callback(int error, const char* description);
        static void check_vk_result(VkResult err);
        #ifdef APP_USE_VULKAN_DEBUG_REPORT
        static VKAPI_ATTR VkBool32 VKAPI_CALL debug_report(VkDebugReportFlagsEXT flags, VkDebugReportObjectTypeEXT objectType, uint64_t object, size_t location, int32_t messageCode, const char* pLayerPrefix, const char* pMessage, void* pUserData);
        #endif // APP_USE_VULKAN_DEBUG_REPORT
        static bool IsExtensionAvailable(const ImVector<VkExtensionProperties>& properties, const char* extension);
        static VkPhysicalDevice SetupVulkan_SelectPhysicalDevice();
        static void SetupVulkan(ImVector<const char*> instance_extensions);
        // All the ImGui_ImplVulkanH_XXX structures/functions are optional helpers used by the demo.
        // Your real engine/app may not use them.
        static void SetupVulkanWindow(ImGui_ImplVulkanH_Window* wd, VkSurfaceKHR surface, int width, int height);
        static void CleanupVulkan();
        static void CleanupVulkanWindow();
        static void FrameRender(ImGui_ImplVulkanH_Window* wd, ImDrawData* draw_data);
        static void FramePresent(ImGui_ImplVulkanH_Window* wd);

    public:
        static int Run();
        static void AddLayer(std::shared_ptr<Layer> layer_ptr);
        static void RemoveLayer(std::shared_ptr<Layer> layer_ptr);
    };
}