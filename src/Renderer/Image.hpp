#pragma once

class Context;
class Buffer;

class Image
{
public:
    struct ImageInfo
    {
        VkFormat            format;
        VkImageLayout       initialLayout;
        VkExtent2D          dimension;
        VkImageUsageFlags   usageFlags;
        VkImageAspectFlags  aspectFlags;
    };
public:
    Image(Context* context, const ImageInfo& imageInfo);
    ~Image();

    Image(const Image& otherAttachment) = delete;
    Image& operator=(const Image& otherAttachment) = delete;

    Image(Image&&) noexcept = default;
    Image& operator=(Image&&) noexcept = default;
public:
    void ChangeLayout(VkImageLayout newLayout);
    void CopyDataToImage(const Buffer* buffer, VkExtent3D imageExtent);
public:
    inline VkImageView GetImageView() const { return m_ImageView; }
    inline VkImageLayout GetImageLayout() const { return m_ImageLayout; }
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
    VkImageUsageFlags   m_UsageFlags;
    VkImageAspectFlags  m_AspectFlags;
    VmaAllocation       m_Allocation;
    VmaAllocationInfo   m_AllocationInfo;
};
