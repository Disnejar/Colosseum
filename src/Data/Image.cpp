#include "Image.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "../App.h"

using namespace Colosseum;

namespace Colosseum
{
    Image::Image(std::string path)
        : _filepath(path)
    {
        int width, height;
        uint8_t *data = nullptr;

        data = stbi_load(_filepath.c_str(), &Width, &Height, &Channels, 4);

        AllocateMemory(Width * Height * Channels);
        SetData(data);
        stbi_image_free(data);
    }

    Image::Image(int width, int height, const void *data, int channels = 4)
        : Width(width), Height(height), Channels(channels)
    {
        AllocateMemory(Width * Height * Channels);
        if (data)
            SetData(data);
    }

    Image::~Image()
    {
        Release();
    }

    void Image::AllocateMemory(uint64_t size)
    {
        VkDevice device = App::g_Device;

        VkResult err;

        VkFormat vulkanFormat = VK_FORMAT_R8G8B8A8_UNORM;

        // Create the Image
        {
            VkImageCreateInfo info = {};
            info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
            info.imageType = VK_IMAGE_TYPE_2D;
            info.format = vulkanFormat;
            info.extent.width = Width;
            info.extent.height = Height;
            info.extent.depth = 1;
            info.mipLevels = 1;
            info.arrayLayers = 1;
            info.samples = VK_SAMPLE_COUNT_1_BIT;
            info.tiling = VK_IMAGE_TILING_OPTIMAL;
            info.usage = VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;
            info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
            info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
            err = vkCreateImage(device, &info, nullptr, &_vkImage);
            Utils::check_vk_result(err);
            VkMemoryRequirements req;
            vkGetImageMemoryRequirements(device, _vkImage, &req);
            VkMemoryAllocateInfo alloc_info = {};
            alloc_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
            alloc_info.allocationSize = req.size;
            alloc_info.memoryTypeIndex = Utils::GetVulkanMemoryType(VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, req.memoryTypeBits);
            err = vkAllocateMemory(device, &alloc_info, nullptr, &_vkImageMemory);
            Utils::check_vk_result(err);
            err = vkBindImageMemory(device, _vkImage, _vkImageMemory, 0);
            Utils::check_vk_result(err);
        }

        // Create the Image View:
        {
            VkImageViewCreateInfo info = {};
            info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
            info.image = _vkImage;
            info.viewType = VK_IMAGE_VIEW_TYPE_2D;
            info.format = vulkanFormat;
            info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            info.subresourceRange.levelCount = 1;
            info.subresourceRange.layerCount = 1;
            err = vkCreateImageView(device, &info, nullptr, &_vkImageView);
            Utils::check_vk_result(err);
        }

        // Create sampler:
        {
            VkSamplerCreateInfo info = {};
            info.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
            info.magFilter = VK_FILTER_LINEAR;
            info.minFilter = VK_FILTER_LINEAR;
            info.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
            info.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
            info.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
            info.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
            info.minLod = -1000;
            info.maxLod = 1000;
            info.maxAnisotropy = 1.0f;
            VkResult err = vkCreateSampler(device, &info, nullptr, &_vkSampler);
            Utils::check_vk_result(err);
        }

        // Create the Descriptor Set:
        _vkDescriptorSet = (VkDescriptorSet)ImGui_ImplVulkan_AddTexture(_vkSampler, _vkImageView, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
    }

    void Image::Release()
    {
        App::SubmitResourceFree(
            [
                sampler = _vkSampler, 
                imageView = _vkImageView, 
                image = _vkImage,
                memory = _vkImageMemory, 
                stagingBuffer = _vkUploadBuffer, 
                stagingBufferMemory = _vkUploadBufferMemory
            ](){
                VkDevice device = App::g_Device;

                vkDestroySampler(device, sampler, nullptr);
                vkDestroyImageView(device, imageView, nullptr);
                vkDestroyImage(device, image, nullptr);
                vkFreeMemory(device, memory, nullptr);
                vkDestroyBuffer(device, stagingBuffer, nullptr);
                vkFreeMemory(device, stagingBufferMemory, nullptr); 
            });

        _vkSampler = nullptr;
        _vkImageView = nullptr;
        _vkImage = nullptr;
        _vkImageMemory = nullptr;
        _vkUploadBuffer = nullptr;
        _vkUploadBufferMemory = nullptr;
    }

    void Image::SetData(const void *data)
    {
        VkDevice device = App::g_Device;

        size_t upload_size = Width * Height * 4;

        VkResult err;

        if (!_vkUploadBuffer)
        {
            // Create the Upload Buffer
            {
                VkBufferCreateInfo buffer_info = {};
                buffer_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
                buffer_info.size = upload_size;
                buffer_info.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
                buffer_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
                err = vkCreateBuffer(device, &buffer_info, nullptr, &_vkUploadBuffer);
                Utils::check_vk_result(err);
                VkMemoryRequirements req;
                vkGetBufferMemoryRequirements(device, _vkUploadBuffer, &req);
                _alignedSize = req.size;
                VkMemoryAllocateInfo alloc_info = {};
                alloc_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
                alloc_info.allocationSize = req.size;
                alloc_info.memoryTypeIndex = Utils::GetVulkanMemoryType(VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT, req.memoryTypeBits);
                err = vkAllocateMemory(device, &alloc_info, nullptr, &_vkUploadBufferMemory);
                Utils::check_vk_result(err);
                err = vkBindBufferMemory(device, _vkUploadBuffer, _vkUploadBufferMemory, 0);
                Utils::check_vk_result(err);
            }
        }

        // Upload to Buffer
        {
            char *map = NULL;
            err = vkMapMemory(device, _vkUploadBufferMemory, 0, _alignedSize, 0, (void **)(&map));
            Utils::check_vk_result(err);
            memcpy(map, data, upload_size);
            VkMappedMemoryRange range[1] = {};
            range[0].sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
            range[0].memory = _vkUploadBufferMemory;
            range[0].size = _alignedSize;
            err = vkFlushMappedMemoryRanges(device, 1, range);
            Utils::check_vk_result(err);
            vkUnmapMemory(device, _vkUploadBufferMemory);
        }

        // Copy to Image
        {
            VkCommandBuffer command_buffer = App::GetCommandBuffer(true);

            VkImageMemoryBarrier copy_barrier = {};
            copy_barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
            copy_barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
            copy_barrier.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
            copy_barrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
            copy_barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
            copy_barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
            copy_barrier.image = _vkImage;
            copy_barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            copy_barrier.subresourceRange.levelCount = 1;
            copy_barrier.subresourceRange.layerCount = 1;
            vkCmdPipelineBarrier(command_buffer, VK_PIPELINE_STAGE_HOST_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0, 0, NULL, 0, NULL, 1, &copy_barrier);

            VkBufferImageCopy region = {};
            region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            region.imageSubresource.layerCount = 1;
            region.imageExtent.width = Width;
            region.imageExtent.height = Height;
            region.imageExtent.depth = 1;
            vkCmdCopyBufferToImage(command_buffer, _vkUploadBuffer, _vkImage, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);

            VkImageMemoryBarrier use_barrier = {};
            use_barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
            use_barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
            use_barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
            use_barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
            use_barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
            use_barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
            use_barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
            use_barrier.image = _vkImage;
            use_barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            use_barrier.subresourceRange.levelCount = 1;
            use_barrier.subresourceRange.layerCount = 1;
            vkCmdPipelineBarrier(command_buffer, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0, 0, NULL, 0, NULL, 1, &use_barrier);

            App::FlushCommandBuffer(command_buffer);
        }
    }

    void Image::Resize(int width, int height)
    {
        if (_vkImage && Width == width && Height == height)
            return;

        // TODO: max size?

        Width = width;
        Height = height;

        Release();
        AllocateMemory(Width * Height * 4);
    }
}