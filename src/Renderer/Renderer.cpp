#include "Core/CnPch.hpp"
#include "Renderer.hpp"

#include "Context.hpp"

Renderer::Renderer(Context* context)
    :   m_Context{context}, m_Swapchain{context},
        m_CommandBuffers{}, m_InFlightFences{},
        m_ImageAvailableSems{}, m_PresentSems{},
        m_ImageIndex{}, m_FrameIndex{}
{
    Init();
}

void Renderer::Init()
{
    CreateCommandBuffers();
    CreateSyncResources();
    CreateGeometryPipeline();
}

void Renderer::CreateCommandBuffers()
{
    VkCommandBufferAllocateInfo cmdBufferInfo{};
    cmdBufferInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    cmdBufferInfo.commandPool = m_Context->GetCommandPool();
    cmdBufferInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    cmdBufferInfo.commandBufferCount = Swapchain::FRAMES_IN_FLIGHT;

    VK_CHECK(vkAllocateCommandBuffers(m_Context->GetLogicalDevice(), &cmdBufferInfo, m_CommandBuffers.data()))
}

void Renderer::CreateSyncResources()
{
    // Fences
    VkFenceCreateInfo fenceInfo{};
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    // Semaphores
    VkSemaphoreCreateInfo semaphoreInfo{};
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    for(uint32_t i = 0; i < Swapchain::FRAMES_IN_FLIGHT; i++)
    {
        VK_CHECK(vkCreateFence(m_Context->GetLogicalDevice(), &fenceInfo, nullptr, &m_InFlightFences[i]))
        VK_CHECK(vkCreateSemaphore(m_Context->GetLogicalDevice(), &semaphoreInfo, nullptr, &m_ImageAvailableSems[i]))
        VK_CHECK(vkCreateSemaphore(m_Context->GetLogicalDevice(), &semaphoreInfo, nullptr, &m_PresentSems[i]))
    }
}

/*
 *  One Pass, Forward Render
 */
void Renderer::CreateGeometryPipeline()
{
    Pipeline::PipelineInfo pipeInfo{};
    pipeInfo.vertexPath     = "/Shaders/GeometryVert.spv";
    pipeInfo.fragmentPath   = "/Shaders/GeometryFrag.spv";
    pipeInfo.colorFormats   = { m_Swapchain.GetFormat() };
    pipeInfo.extent         = m_Swapchain.GetExtent();

    m_GeometryPipeline = std::make_unique<Pipeline>(m_Context, pipeInfo);
}

void Renderer::BeginFrame()
{
    vkWaitForFences(m_Context->GetLogicalDevice(), 1, &m_InFlightFences[m_FrameIndex], VK_TRUE, std::numeric_limits<uint64_t>::max());
    VK_CHECK(vkAcquireNextImageKHR(m_Context->GetLogicalDevice(), m_Swapchain.GetSwapchain(), UINT64_MAX, m_ImageAvailableSems[m_FrameIndex], VK_NULL_HANDLE, &m_ImageIndex))

    /*
     *  Handle Swapchain Recreation
     */

    vkResetFences(m_Context->GetLogicalDevice(), 1U, &m_InFlightFences[m_FrameIndex]);
    vkResetCommandBuffer(m_CommandBuffers[m_FrameIndex], 0U);

    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

    VK_CHECK(vkBeginCommandBuffer(m_CommandBuffers[m_FrameIndex], &beginInfo))
}

void Renderer::EndFrame()
{
    m_Swapchain.ChangeLayout(m_ImageIndex, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR, VK_IMAGE_ASPECT_COLOR_BIT);
    VK_CHECK(vkEndCommandBuffer(m_CommandBuffers[m_FrameIndex]))

    VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT  };

    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = &m_ImageAvailableSems[m_FrameIndex];
    submitInfo.pWaitDstStageMask = waitStages;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &m_CommandBuffers[m_FrameIndex];
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = &m_PresentSems[m_FrameIndex];

    VK_CHECK(vkQueueSubmit(m_Context->GetGraphicsQueue(), 1, &submitInfo, m_InFlightFences[m_FrameIndex]))

    VkSwapchainKHR swapchain[] = {m_Swapchain.GetSwapchain()};

    VkPresentInfoKHR presentInfo{};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = &m_PresentSems[m_FrameIndex];
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = swapchain;
    presentInfo.pImageIndices = &m_ImageIndex;

    VK_CHECK(vkQueuePresentKHR(m_Context->GetPresentQueue(), &presentInfo))
    m_FrameIndex = (m_FrameIndex + 1) % Swapchain::FRAMES_IN_FLIGHT;
}

void Renderer::DrawFrame()
{
    BeginFrame();
    GeometryPass();
    EndFrame();
}

/*
 * One Pass, Forward Render
 * Rendering directly into swapchain images
 *
 * Deferred renderer will render into separate buffers later
 */
void Renderer::GeometryPass()
{
    /*
     * Transition Image Layout to Color Attachment Optimal
     * Call Pipeline Render Function
     * Call Pipeline End Render Function
     */
    m_Swapchain.ChangeLayout(m_ImageIndex, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, VK_IMAGE_ASPECT_COLOR_BIT);

    Pipeline::Attachment colorAttachment{};
    colorAttachment.imageView   = m_Swapchain.GetImageViews()[m_ImageIndex];
    colorAttachment.imageLayout = m_Swapchain.GetImageLayouts()[m_ImageIndex];
    colorAttachment.loadOp      = VK_ATTACHMENT_LOAD_OP_CLEAR;
    colorAttachment.storeOp     = VK_ATTACHMENT_STORE_OP_STORE;
    colorAttachment.clearValue = {};

    Pipeline::RenderInfo renderInfo{};
    renderInfo.colorAttachments = { colorAttachment };
    renderInfo.extent = m_Swapchain.GetExtent();

    m_GeometryPipeline->BeginRender(m_CommandBuffers[m_FrameIndex], renderInfo);
    m_GeometryPipeline->Draw(3);
    m_GeometryPipeline->EndRender();
}

Renderer::~Renderer()
{
    vkDeviceWaitIdle(m_Context->GetLogicalDevice());

    if(!m_CommandBuffers.empty())
    {
        vkFreeCommandBuffers(m_Context->GetLogicalDevice(), m_Context->GetCommandPool(), m_CommandBuffers.size(), m_CommandBuffers.data());
    }
    if(!m_InFlightFences.empty())
    {
        for(VkFence fence : m_InFlightFences)
        {
            vkDestroyFence(m_Context->GetLogicalDevice(), fence, nullptr);
        }
    }
    if(!m_ImageAvailableSems.empty())
    {
        for(VkSemaphore sem : m_ImageAvailableSems)
        {
            vkDestroySemaphore(m_Context->GetLogicalDevice(), sem, nullptr);
        }
    }
    if(!m_PresentSems.empty())
    {
        for(VkSemaphore sem : m_PresentSems)
        {
            vkDestroySemaphore(m_Context->GetLogicalDevice(), sem, nullptr);
        }
    }
}