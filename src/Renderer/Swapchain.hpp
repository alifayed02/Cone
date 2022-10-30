#pragma once

class Context;

class Swapchain
{
public:
    struct SwapchainSupportDetails
    {
        VkSurfaceCapabilitiesKHR        capabilities;
        std::vector<VkSurfaceFormatKHR> formats;
        std::vector<VkPresentModeKHR>   presentModes;
    };
public:
    explicit Swapchain(Context* context);
    ~Swapchain();

    Swapchain(const Swapchain& otherSwapchain) = delete;
    Swapchain& operator=(const Swapchain& otherSwapchain) = delete;
public:
    void ChangeLayout(size_t imageIndex, VkImageLayout newLayout, VkImageAspectFlags aspectFlags);
    void ChangeLayout(size_t imageIndex, VkImageLayout newLayout, VkImageAspectFlags aspectFlags, VkCommandBuffer);
public:
    inline VkFormat GetFormat() const { return m_ImageFormat; }
    inline VkSwapchainKHR GetSwapchain() const { return m_Swapchain; }
    inline VkExtent2D GetExtent() const { return m_Extent; }
    inline const std::vector<VkImageView> GetImageViews() const { return m_ImageViews; }
    inline const std::vector<VkImageLayout> GetImageLayouts() const { return m_ImageLayouts; }
    inline static constexpr uint32_t FRAMES_IN_FLIGHT = 2;
private:
    void Init();
    SwapchainSupportDetails QuerySwapchainSupport();
private:
    Context*                    m_Context;
    VkSwapchainKHR              m_Swapchain;
    VkFormat                    m_ImageFormat;
    VkExtent2D                  m_Extent;
    std::vector<VkImage>        m_Images;
    std::vector<VkImageView>    m_ImageViews;
    std::vector<VkImageLayout>  m_ImageLayouts;
};