#include "Core/CnPch.hpp"
#include "Image.hpp"

#include "Context.hpp"
#include "Buffer/Buffer.hpp"

Image::Image(Context* context, const ImageInfo& imageInfo)
    :   m_Context{context}, m_Image{}, m_ImageView{},
        m_ImageLayout{VK_IMAGE_LAYOUT_UNDEFINED}, m_ImageFormat{imageInfo.format},
        m_ImageDimension{imageInfo.dimension}, m_GenMipmaps{imageInfo.genMipmaps}, m_ImageMipLevels{1}, m_UsageFlags{imageInfo.usageFlags},
        m_AspectFlags{imageInfo.aspectFlags}, m_Allocation{}, m_AllocationInfo{}
{
    CreateImage();
    CreateImageView();
    ChangeLayout(imageInfo.desiredLayout);
}

void Image::ChangeLayout(VkImageLayout newLayout, VkPipelineStageFlags sourceFlags)
{
    if(m_ImageLayout == newLayout)
    {
        return;
    }

    VkCommandBuffer commandBuffer = m_Context->BeginSingleTimeCommands(Context::CommandType::GRAPHICS);

    Utilities::LayoutTransitionInfo transitionInfo{};
    transitionInfo.oldLayout        = m_ImageLayout;
    transitionInfo.newLayout        = newLayout;
    transitionInfo.image            = m_Image;
    transitionInfo.mipLevels        = m_ImageMipLevels;
    transitionInfo.aspectFlags      = m_AspectFlags;
    transitionInfo.sourceStageFlags = sourceFlags;

    Utilities::ChangeLayout(commandBuffer, transitionInfo);
    m_Context->EndSingleTimeCommands(Context::CommandType::GRAPHICS, commandBuffer);

    m_ImageLayout = newLayout;
}

void Image::CopyDataToImage(const Buffer* buffer, const VkExtent3D imageExtent)
{
    VkCommandBuffer commandBuffer = m_Context->BeginSingleTimeCommands(Context::CommandType::TRANSFER);

    VkBufferImageCopy region{};
    region.bufferOffset = 0;
    region.bufferRowLength = 0;
    region.bufferImageHeight = 0;

    region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    region.imageSubresource.mipLevel = 0;
    region.imageSubresource.baseArrayLayer = 0;
    region.imageSubresource.layerCount = 1;

    region.imageOffset = { 0, 0, 0 };
    region.imageExtent = imageExtent;

    vkCmdCopyBufferToImage(
            commandBuffer,
            buffer->GetBuffer(),
            m_Image,
            VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
            1,
            &region
    );

    m_Context->EndSingleTimeCommands(Context::CommandType::TRANSFER, commandBuffer);
}

void Image::CreateImage()
{
    if(m_GenMipmaps == VK_TRUE)
    {
        m_ImageMipLevels = static_cast<uint32_t>(std::floor(std::log2(std::max(m_ImageDimension.width, m_ImageDimension.height)))) + 1;
        m_UsageFlags |= VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
    }

    VkImageCreateInfo imageCreateInfo{};
    imageCreateInfo.sType           = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    imageCreateInfo.imageType       = VK_IMAGE_TYPE_2D;
    imageCreateInfo.format          = m_ImageFormat;
    imageCreateInfo.extent.width    = m_ImageDimension.width;
    imageCreateInfo.extent.height   = m_ImageDimension.height;
    imageCreateInfo.extent.depth    = 1U;
    imageCreateInfo.mipLevels       = m_ImageMipLevels;
    imageCreateInfo.arrayLayers     = 1U;
    imageCreateInfo.samples         = VK_SAMPLE_COUNT_1_BIT;
    imageCreateInfo.tiling          = VK_IMAGE_TILING_OPTIMAL;
    imageCreateInfo.usage           = m_UsageFlags;
    imageCreateInfo.sharingMode     = VK_SHARING_MODE_EXCLUSIVE;
    imageCreateInfo.initialLayout   = m_ImageLayout;

    VmaAllocationCreateInfo vmaAllocationCreateInfo{};
    vmaAllocationCreateInfo.usage = VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE;
    vmaAllocationCreateInfo.requiredFlags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;

    VK_CHECK(vmaCreateImage(m_Context->GetAllocator(), &imageCreateInfo, &vmaAllocationCreateInfo, &m_Image, &m_Allocation, &m_AllocationInfo))
}

void Image::CreateImageView()
{
    VkImageViewCreateInfo imageViewCreateInfo{};
    imageViewCreateInfo.sType                           = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    imageViewCreateInfo.image                           = m_Image;
    imageViewCreateInfo.viewType                        = VK_IMAGE_VIEW_TYPE_2D;
    imageViewCreateInfo.format                          = m_ImageFormat;
    imageViewCreateInfo.subresourceRange.aspectMask     = m_AspectFlags;
    imageViewCreateInfo.subresourceRange.baseMipLevel   = 0;
    imageViewCreateInfo.subresourceRange.levelCount     = m_ImageMipLevels;
    imageViewCreateInfo.subresourceRange.baseArrayLayer = 0;
    imageViewCreateInfo.subresourceRange.layerCount     = 1;

    VK_CHECK(vkCreateImageView(m_Context->GetLogicalDevice(), &imageViewCreateInfo, nullptr, &m_ImageView))
}

void Image::GenerateMipmaps(VkImageLayout finalLayout)
{
    VkFormatProperties formatProperties;
    vkGetPhysicalDeviceFormatProperties(m_Context->GetPhysicalDevice(), m_ImageFormat, &formatProperties);

    if (!(formatProperties.optimalTilingFeatures & VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT))
    {
        throw std::runtime_error("Error: Image format does not support linear blitting.");
    }

    VkCommandBuffer commandBuffer = m_Context->BeginSingleTimeCommands(Context::CommandType::GRAPHICS);

    VkImageMemoryBarrier barrier{};
    barrier.sType                           = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    barrier.image                           = m_Image;
    barrier.srcQueueFamilyIndex             = VK_QUEUE_FAMILY_IGNORED;
    barrier.dstQueueFamilyIndex             = VK_QUEUE_FAMILY_IGNORED;
    barrier.subresourceRange.aspectMask     = m_AspectFlags;
    barrier.subresourceRange.baseArrayLayer = 0;
    barrier.subresourceRange.layerCount     = 1;
    barrier.subresourceRange.levelCount     = 1;

    auto mipWidth    = static_cast<int32_t>(m_ImageDimension.width);
    auto mipHeight   = static_cast<int32_t>(m_ImageDimension.height);

    for (uint32_t i = 1; i < m_ImageMipLevels; i++)
    {
        barrier.subresourceRange.baseMipLevel = i - 1;
        barrier.oldLayout       = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
        barrier.newLayout       = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
        barrier.srcAccessMask   = VK_ACCESS_TRANSFER_WRITE_BIT;
        barrier.dstAccessMask   = VK_ACCESS_TRANSFER_READ_BIT;

        vkCmdPipelineBarrier(
                    commandBuffer,
                    VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0,
                    0, nullptr,
                    0, nullptr,
                    1, &barrier
                );

        VkImageBlit blit{};
        blit.srcOffsets[0]                  = { 0, 0, 0 };
        blit.srcOffsets[1]                  = { mipWidth, mipHeight, 1 };
        blit.srcSubresource.aspectMask      = m_AspectFlags;
        blit.srcSubresource.mipLevel        = i - 1;
        blit.srcSubresource.baseArrayLayer  = 0;
        blit.srcSubresource.layerCount      = 1;
        blit.dstOffsets[0]                  = { 0, 0, 0 };
        blit.dstOffsets[1]                  = { mipWidth > 1 ? mipWidth / 2 : 1, mipHeight > 1 ? mipHeight / 2 : 1, 1 };
        blit.dstSubresource.aspectMask      = m_AspectFlags;
        blit.dstSubresource.mipLevel        = i;
        blit.dstSubresource.baseArrayLayer  = 0;
        blit.dstSubresource.layerCount      = 1;

        vkCmdBlitImage(
                    commandBuffer,
                    m_Image, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
                    m_Image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                    1, &blit,
                    VK_FILTER_LINEAR
                );

        barrier.oldLayout       = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
        barrier.newLayout       = finalLayout;
        barrier.srcAccessMask   = VK_ACCESS_TRANSFER_READ_BIT;
        barrier.dstAccessMask   = VK_ACCESS_SHADER_READ_BIT;

        vkCmdPipelineBarrier(
                    commandBuffer,
                    VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0,
                    0, nullptr,
                    0, nullptr,
                    1, &barrier
                );

        if (mipWidth > 1) mipWidth /= 2;
        if (mipHeight > 1) mipHeight /= 2;
    }

    barrier.subresourceRange.baseMipLevel   = m_ImageMipLevels - 1;
    barrier.oldLayout                       = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
    barrier.newLayout                       = finalLayout;
    barrier.srcAccessMask                   = VK_ACCESS_TRANSFER_WRITE_BIT;
    barrier.dstAccessMask                   = VK_ACCESS_SHADER_READ_BIT;

    vkCmdPipelineBarrier(
            commandBuffer,
            VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0,
            0, nullptr,
            0, nullptr,
            1, &barrier
    );

    m_Context->EndSingleTimeCommands(Context::CommandType::GRAPHICS, commandBuffer);

    m_ImageLayout = finalLayout;
}

Image::~Image()
{
    if(m_ImageView)
    {
        vkDestroyImageView(m_Context->GetLogicalDevice(), m_ImageView, nullptr);
    }
    if(m_Image)
    {
        vmaDestroyImage(m_Context->GetAllocator(), m_Image, m_Allocation);
    }
}
