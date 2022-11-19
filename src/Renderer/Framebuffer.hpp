#pragma once

#include "Image.hpp"

class Context;

class Framebuffer
{
public:
    struct AttachmentInfo
    {
        VkFormat            format;
        VkImageLayout       layout;
        VkImageUsageFlags   usageFlags;
        VkImageAspectFlags  aspectFlags;
    };
public:
    Framebuffer(Context* context, VkExtent2D dimension, const std::vector<AttachmentInfo>& attachments);
    ~Framebuffer();

    Framebuffer(const Framebuffer& otherFramebuffer) = delete;
    Framebuffer& operator=(const Framebuffer& otherFrameBuffer) = delete;
public:
    inline std::vector<Image>& GetAttachments() { return m_Attachments; }
    inline VkSampler GetSampler() const { return m_Sampler; }
private:
    void CreateSampler();
private:
    Context*            m_Context;
    VkExtent2D          m_Dimension;
    std::vector<Image>  m_Attachments;
    VkSampler           m_Sampler;
};
