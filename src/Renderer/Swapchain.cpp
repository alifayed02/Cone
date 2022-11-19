#include "Core/CnPch.hpp"
#include "Swapchain.hpp"

#include "Context.hpp"

Swapchain::Swapchain(Context* context)
        : m_Context{context}, m_Swapchain{}, m_ImageFormat{}, m_Extent{}
{
    Init();
}

void Swapchain::Init()
{
    SwapchainSupportDetails support = QuerySwapchainSupport();

    vkb::SwapchainBuilder swapchainBuilder{m_Context->GetPhysicalDevice(), m_Context->GetLogicalDevice(), m_Context->GetSurface()};
    vkb::Swapchain vkbSwapchain = swapchainBuilder
            .set_desired_min_image_count(support.capabilities.minImageCount + 1)
            .set_desired_format({ .format=VK_FORMAT_B8G8R8A8_SRGB, .colorSpace=VK_COLOR_SPACE_SRGB_NONLINEAR_KHR })
            .set_desired_present_mode(VK_PRESENT_MODE_FIFO_KHR)
            .set_desired_extent(m_Context->GetSurfaceExtent().width, m_Context->GetSurfaceExtent().height)
            .build()
            .value();
    m_Swapchain     = vkbSwapchain.swapchain;
    m_ImageFormat   = vkbSwapchain.image_format;
    m_Extent        = vkbSwapchain.extent;
    m_Images        = vkbSwapchain.get_images().value();
    m_ImageViews    = vkbSwapchain.get_image_views().value();
    m_ImageLayouts.resize(m_ImageViews.size(), VK_IMAGE_LAYOUT_UNDEFINED);
}

Swapchain::SwapchainSupportDetails Swapchain::QuerySwapchainSupport()
{
    SwapchainSupportDetails details;

    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(m_Context->GetPhysicalDevice(), m_Context->GetSurface(), &details.capabilities);

    uint32_t formatCount;
    vkGetPhysicalDeviceSurfaceFormatsKHR(m_Context->GetPhysicalDevice(), m_Context->GetSurface(), &formatCount, nullptr);

    if (formatCount != 0)
    {
        details.formats.resize(formatCount);
        vkGetPhysicalDeviceSurfaceFormatsKHR(m_Context->GetPhysicalDevice(), m_Context->GetSurface(), &formatCount, details.formats.data());
    }

    uint32_t presentModeCount;
    vkGetPhysicalDeviceSurfacePresentModesKHR(m_Context->GetPhysicalDevice(), m_Context->GetSurface(), &presentModeCount, nullptr);

    if (presentModeCount != 0)
    {
        details.presentModes.resize(presentModeCount);
        vkGetPhysicalDeviceSurfacePresentModesKHR(m_Context->GetPhysicalDevice(), m_Context->GetSurface(), &presentModeCount, details.presentModes.data());
    }

    return details;
}

void Swapchain::ChangeLayout(size_t imageIndex, VkImageLayout newLayout, VkImageAspectFlags aspectFlags)
{
    VkCommandBuffer commandBuffer = m_Context->BeginSingleTimeCommands(Context::CommandType::GRAPHICS);
    Utilities::ChangeLayout(commandBuffer, m_ImageLayouts[imageIndex], newLayout, m_Images[imageIndex], aspectFlags);
    m_Context->EndSingleTimeCommands(Context::CommandType::GRAPHICS, commandBuffer);
    m_ImageLayouts[imageIndex] = newLayout;
}

void Swapchain::ChangeLayout(size_t imageIndex, VkImageLayout newLayout, VkImageAspectFlags aspectFlags, VkCommandBuffer commandBuffer)
{
    Utilities::ChangeLayout(commandBuffer, m_ImageLayouts[imageIndex], newLayout, m_Images[imageIndex], aspectFlags);
    m_ImageLayouts[imageIndex] = newLayout;
}

Swapchain::~Swapchain()
{
    for(auto imageView : m_ImageViews)
    {
        vkDestroyImageView(m_Context->GetLogicalDevice(), imageView, nullptr);
    }

    if(m_Swapchain)
    {
        vkDestroySwapchainKHR(m_Context->GetLogicalDevice(), m_Swapchain, nullptr);
    }
}