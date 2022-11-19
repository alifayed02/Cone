#include "Core/CnPch.hpp"
#include "Framebuffer.hpp"

#include "Context.hpp"

Framebuffer::Framebuffer(Context* context, VkExtent2D dimension, const std::vector<AttachmentInfo>& attachments)
    :   m_Context{context}, m_Dimension{dimension}
{
    m_Attachments.reserve(attachments.size());
    for(const AttachmentInfo& attachment : attachments)
    {
        Image::ImageInfo imageInfo{};
        imageInfo.format        = attachment.format;
        imageInfo.desiredLayout = attachment.layout;
        imageInfo.dimension     = m_Dimension;
        imageInfo.usageFlags    = attachment.usageFlags;
        imageInfo.aspectFlags   = attachment.aspectFlags;

        m_Attachments.emplace_back(context, imageInfo);
    }

    CreateSampler();
}

void Framebuffer::CreateSampler()
{
    VkSamplerCreateInfo samplerCreateInfo{};
    samplerCreateInfo.sType             = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
    samplerCreateInfo.magFilter         = VK_FILTER_LINEAR;
    samplerCreateInfo.minFilter         = VK_FILTER_LINEAR;
    samplerCreateInfo.addressModeU      = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    samplerCreateInfo.addressModeV      = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    samplerCreateInfo.addressModeW      = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    samplerCreateInfo.anisotropyEnable  = VK_TRUE;

    VkPhysicalDeviceProperties properties{};
    vkGetPhysicalDeviceProperties(m_Context->GetPhysicalDevice(), &properties);

    samplerCreateInfo.maxAnisotropy             = properties.limits.maxSamplerAnisotropy;
    samplerCreateInfo.borderColor               = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
    samplerCreateInfo.unnormalizedCoordinates   = VK_FALSE;
    samplerCreateInfo.compareEnable             = VK_FALSE;
    samplerCreateInfo.compareOp                 = VK_COMPARE_OP_ALWAYS;
    samplerCreateInfo.mipmapMode                = VK_SAMPLER_MIPMAP_MODE_LINEAR;
    samplerCreateInfo.mipLodBias                = 0.0f;
    samplerCreateInfo.minLod                    = 0.0f;
    samplerCreateInfo.maxLod                    = 0.0f;

    VK_CHECK(vkCreateSampler(m_Context->GetLogicalDevice(), &samplerCreateInfo, nullptr, &m_Sampler))
}

Framebuffer::~Framebuffer()
{
    if(m_Sampler)
    {
        vkDestroySampler(m_Context->GetLogicalDevice(), m_Sampler, nullptr);
    }
}
