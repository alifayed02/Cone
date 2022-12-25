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
        uint32_t            numLayers;
        VkBool32            genMipmaps;
    };
public:
    Image(Context* context, const ImageInfo& imageInfo);
    ~Image();

    Image(Image&& otherImage) noexcept = default;
    Image& operator=(Image&& otherImage) noexcept = default;

    Image(const Image& otherImage) = delete;
    Image& operator=(const Image& otherImage) = delete;
public:
    void ChangeLayout(VkImageLayout newLayout, VkPipelineStageFlags sourceFlags = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT);
    void CopyDataToImage(const Buffer* buffer, VkExtent3D imageExtent);
    void GenerateMipmaps(VkImageLayout finalLayout);
public:
    inline VkImage GetImage() const { return m_Image; }
    inline VkImageView GetImageView(const uint32_t layer) const { return m_ImageViews[layer]; }
    inline VkImageLayout GetImageLayout() const { return m_ImageLayout; }
    inline VkFormat GetImageFormat() const { return m_ImageFormat; }
    inline VkExtent2D GetImageExtent() const { return m_ImageDimension; }
    inline uint32_t GetLayerCount() const { return m_ImageLayers; }
    inline float GetMaxLOD() const { return m_GenMipmaps == VK_TRUE ? static_cast<float>(m_ImageMipLevels) : 0.0f; }
private:
    void CreateImage();
    void CreateImageView();
private:
    Context*                    m_Context;
    VkImage                     m_Image;
    std::vector<VkImageView>    m_ImageViews;
    VkImageLayout               m_ImageLayout;
    VkFormat                    m_ImageFormat;
    VkExtent2D                  m_ImageDimension;
    VkBool32                    m_GenMipmaps;
    uint32_t                    m_ImageMipLevels;
    uint32_t                    m_ImageLayers;
    VkImageUsageFlags           m_UsageFlags;
    VkImageAspectFlags          m_AspectFlags;
    VmaAllocation               m_Allocation;
    VmaAllocationInfo           m_AllocationInfo;
};
