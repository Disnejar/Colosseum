#pragma once

#include <string>
#include "imgui_impl_vulkan.h"
#include "../Utils.h"

namespace Colosseum {
    class Image{
    public:
        int Width = 0;
        int Height = 0;
        int Channels = 4;

    private:
        std::string             _filepath;
        size_t                  _alignedSize;

        VkDescriptorSet         _vkDescriptorSet        = nullptr;
        VkImageView             _vkImageView            = nullptr;
        VkImage                 _vkImage                = nullptr;
        VkDeviceMemory          _vkImageMemory          = nullptr;
        VkSampler               _vkSampler              = nullptr;
        VkBuffer                _vkUploadBuffer         = nullptr;
        VkDeviceMemory          _vkUploadBufferMemory   = nullptr;

    
    public:
        Image(std::string path);
        Image(int width, int height, const void *data, int channels);
        ~Image();

        VkDescriptorSet GetDescriptor() const { return _vkDescriptorSet; };
        void SetData(const void *data);
        void Resize(int width, int height);

    private:
        void AllocateMemory(uint64_t size);
        void Release();
    };
}