#include "Core/CnPch.hpp"
#include "Image.hpp"

#include "Context.hpp"

Image::Image(Context* context, const ImageInfo& imageInfo)
    :   m_Context{context}, m_Image{}, m_ImageView{},
        m_ImageLayout{VK_IMAGE_LAYOUT_UNDEFINED}, m_ImageDimension{imageInfo.dimension},
        m_ImageFormat{imageInfo.format}, m_ImageMemory{}, m_UsageFlags{imageInfo.usageFlags},
        m_AspectFlags{imageInfo.aspectFlags}
{
    CreateImage();
    CreateImageView();
}

void Image::ChangeLayout(VkImageLayout newLayout)
{
    VkCommandBuffer commandBuffer = m_Context->BeginSingleTimeCommands();

    VkImageMemoryBarrier barrier{};
    barrier.sType                           = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    barrier.oldLayout                       = m_ImageLayout;
    barrier.newLayout                       = newLayout;
    barrier.srcQueueFamilyIndex             = VK_QUEUE_FAMILY_IGNORED;
    barrier.dstQueueFamilyIndex             = VK_QUEUE_FAMILY_IGNORED;
    barrier.image                           = m_Image;
    barrier.subresourceRange.aspectMask     = m_AspectFlags;
    barrier.subresourceRange.baseMipLevel   = 0;
    barrier.subresourceRange.levelCount     = 1;
    barrier.subresourceRange.baseArrayLayer = 0;
    barrier.subresourceRange.layerCount     = 1;

    VkPipelineStageFlags sourceStage;
    VkPipelineStageFlags destinationStage;

    if(m_ImageLayout == VK_IMAGE_LAYOUT_UNDEFINED)
    {
        barrier.srcAccessMask = 0;
        sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
    } else if(m_ImageLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL)
    {
        barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
    }
    else {
        throw std::invalid_argument("Error: This layout transition is unsupported!");
    }

    if(newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL)
    {
        barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
    } else if(newLayout == VK_IMAGE_LAYOUT_PRESENT_SRC_KHR)
    {
        barrier.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT;
        destinationStage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    } else if(newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
    {
        barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
        destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
    }
    else {
        throw std::invalid_argument("Error: This layout transition is unsupported!");
    }

    vkCmdPipelineBarrier(
            commandBuffer,
            sourceStage, destinationStage,
            0,
            0, nullptr,
            0, nullptr,
            1, &barrier
    );

    m_Context->EndSingleTimeCommands(commandBuffer);
    m_ImageLayout = newLayout;
}

void Image::CreateImage()
{
    VkImageCreateInfo imageCreateInfo{};
    imageCreateInfo.sType           = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    imageCreateInfo.imageType       = VK_IMAGE_TYPE_2D;
    imageCreateInfo.format          = m_ImageFormat;
    imageCreateInfo.extent.width    = m_ImageDimension.width;
    imageCreateInfo.extent.height   = m_ImageDimension.height;
    imageCreateInfo.extent.depth    = 1U;
    imageCreateInfo.mipLevels       = 1U;
    imageCreateInfo.arrayLayers     = 1U;
    imageCreateInfo.samples         = VK_SAMPLE_COUNT_1_BIT;
    imageCreateInfo.tiling          = VK_IMAGE_TILING_OPTIMAL;
    imageCreateInfo.usage           = m_UsageFlags;
    imageCreateInfo.sharingMode     = VK_SHARING_MODE_EXCLUSIVE;
    imageCreateInfo.initialLayout   = VK_IMAGE_LAYOUT_UNDEFINED;

    VK_CHECK(vkCreateImage(m_Context->GetLogicalDevice(), &imageCreateInfo, nullptr, &m_Image))
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
    imageViewCreateInfo.subresourceRange.levelCount     = 1;
    imageViewCreateInfo.subresourceRange.baseArrayLayer = 0;
    imageViewCreateInfo.subresourceRange.layerCount     = 1;

    VK_CHECK(vkCreateImageView(m_Context->GetLogicalDevice(), &imageViewCreateInfo, nullptr, &m_ImageView))
}

Image::~Image()
{
    if(m_ImageView)
    {
        vkDestroyImageView(m_Context->GetLogicalDevice(), m_ImageView, nullptr);
    }
    if(m_Image)
    {
        vkDestroyImage(m_Context->GetLogicalDevice(), m_Image, nullptr);
    }
    if(m_ImageMemory)
    {
        vkFreeMemory(m_Context->GetLogicalDevice(), m_ImageMemory, nullptr);
    }
}
