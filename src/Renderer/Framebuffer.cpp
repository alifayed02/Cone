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
        imageInfo.initialLayout = attachment.layout;
        imageInfo.dimension     = m_Dimension;
        imageInfo.usageFlags    = attachment.usageFlags;
        imageInfo.aspectFlags   = attachment.aspectFlags;

        m_Attachments.emplace_back(context, imageInfo);
    }
}
