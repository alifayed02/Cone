#include "Swapchain.hpp"

#include "Context.hpp"

Swapchain::Swapchain(Context* context)
        :   m_Context{context}, m_Swapchain{},
            m_SwapchainImageFormat{}
{
    Init();
}

void Swapchain::Init()
{
    vkb::SwapchainBuilder swapchainBuilder{m_Context->GetPhysicalDevice(), m_Context->GetLogicalDevice(), m_Context->GetSurface()};
    vkb::Swapchain vkbSwapchain = swapchainBuilder
            .use_default_format_selection()
            .set_desired_present_mode(VK_PRESENT_MODE_FIFO_KHR)
            .set_desired_extent(m_Context->GetSurfaceExtent().width, m_Context->GetSurfaceExtent().height)
            .build()
            .value();
    m_Swapchain = vkbSwapchain.swapchain;
    m_SwapchainImageFormat = vkbSwapchain.image_format;
    m_Images = vkbSwapchain.get_images().value();
    m_ImageViews = vkbSwapchain.get_image_views().value();
}

Swapchain::~Swapchain()
{
    if(m_Swapchain)
    {
        vkDestroySwapchainKHR(m_Context->GetLogicalDevice(), m_Swapchain, nullptr);
    }

    for(auto imageView : m_ImageViews)
    {
        vkDestroyImageView(m_Context->GetLogicalDevice(), imageView, nullptr);
    }

    for(auto image : m_Images)
    {
        vkDestroyImage(m_Context->GetLogicalDevice(), image, nullptr);
    }
}
