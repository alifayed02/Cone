#pragma once

class Context;

class Swapchain
{
public:
    explicit Swapchain(Context* context);
    ~Swapchain();

    Swapchain(const Swapchain& otherSwapchain) = delete;
    Swapchain& operator=(const Swapchain& otherSwapchain) = delete;
public:
    inline VkFormat GetFormat() const { return m_SwapchainImageFormat; }
private:
    void Init();
private:
    Context*                    m_Context;
    VkSwapchainKHR              m_Swapchain;
    VkFormat                    m_SwapchainImageFormat;
    std::vector<VkImage>        m_Images;
    std::vector<VkImageView>    m_ImageViews;
};
