#pragma once

#include "imgui_impl_vulkan.h"
#include <cstdint>

namespace Colosseum
{
    namespace Utils
    {
        uint32_t GetVulkanMemoryType(VkMemoryPropertyFlags properties, uint32_t type_bits);
        void glfw_error_callback(int error, const char *description);
        void check_vk_result(VkResult err);

#ifdef APP_USE_VULKAN_DEBUG_REPORT
        VKAPI_ATTR VkBool32 VKAPI_CALL debug_report(VkDebugReportFlagsEXT flags, VkDebugReportObjectTypeEXT objectType, uint64_t object, size_t location, int32_t messageCode, const char *pLayerPrefix, const char *pMessage, void *pUserData);
#endif // APP_USE_VULKAN_DEBUG_REPORT
    }
}