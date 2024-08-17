#pragma once

#include <string>
#include "imgui_impl_vulkan.h"

namespace Colosseum {
    class Image{
    public:
        uint32_t Width = 0;
        uint32_t Height = 0;
        uint32_t Channels = 4;

    private:
        std::string             _filepath;

        VkDescriptorSet         _vkDescriptorSet        = nullptr;
        VkImageView             _vkImageView            = nullptr;
        VkImage                 _vkImage                = nullptr;
        VkDeviceMemory          _vkImageMemory          = nullptr;
        VkSampler               _vkSampler              = nullptr;
        VkBuffer                _vkUploadBuffer         = nullptr;
        VkDeviceMemory          _vkUploadBufferMemory   = nullptr;
    
    public:
        Image();

    private:

    };
}