#pragma once

class Context;
class Buffer;

class Image
{
public:
    struct ImageInfo
    {
        VkFormat            format;
        VkImageLayout       desiredLayout;
        VkExtent2D          dimension;
        VkImageUsageFlags   usageFlags;
        VkImageAspectFlags  aspectFlags;
        VkBool32            genMipmaps;
    };
public:
    Image(Context* context, const ImageInfo& imageInfo);
    ~Image();

    Image(Image&&) noexcept = default;
    Image& operator=(Image&&) noexcept = default;

    Image(const Image& otherAttachment) = delete;
    Image& operator=(const Image& otherAttachment) = delete;
public:
    void ChangeLayout(VkImageLayout newLayout, VkPipelineStageFlags sourceFlags = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT);
    void CopyDataToImage(const Buffer* buffer, VkExtent3D imageExtent);
    void GenerateMipmaps(VkImageLayout finalLayout);
public:
    inline VkImageView GetImageView() const { return m_ImageView; }
    inline VkImageLayout GetImageLayout() const { return m_ImageLayout; }
    inline VkFormat GetImageFormat() const { return m_ImageFormat; }
    inline float GetMaxLOD() const { return m_GenMipmaps == VK_TRUE ? static_cast<float>(m_ImageMipLevels) : 0.0f; }
private:
    void CreateImage();
    void CreateImageView();
private:
    Context*            m_Context;
    VkImage             m_Image;
    VkImageView         m_ImageView;
    VkImageLayout       m_ImageLayout;
    VkFormat            m_ImageFormat;
    VkExtent2D          m_ImageDimension;
    VkBool32            m_GenMipmaps;
    uint32_t            m_ImageMipLevels;
    VkImageUsageFlags   m_UsageFlags;
    VkImageAspectFlags  m_AspectFlags;
    VmaAllocation       m_Allocation;
    VmaAllocationInfo   m_AllocationInfo;
};
