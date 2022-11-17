#include "Core/CnPch.hpp"
#include "Renderer.hpp"

#include "Scene/Scene.hpp"
#include "Scene/SceneMember.hpp"
#include "Asset/Mesh.hpp"
#include "Asset/Material.hpp"
#include "Context.hpp"

#include "glm/gtc/matrix_transform.hpp"

Renderer::Renderer(Context* context, Scene* scene)
    :   m_Context{context}, m_ActiveScene{scene}, m_Swapchain{context},
        m_CommandBuffers{}, m_InFlightFences{},
        m_ImageAvailableSems{}, m_PresentSems{},
        m_ImageIndex{}, m_FrameIndex{}
{
    Init();
    m_ActiveScene->GetCamera().SetExtent(m_Swapchain.GetExtent());
}

void Renderer::Init()
{
    CreateCommandBuffers();
    CreateSyncResources();
    CreateGeometryPipeline();
    CreateGeometryPassResources();
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
    // REFACTOR
    VkPushConstantRange cameraPushConstant{};
    cameraPushConstant.stageFlags    = VK_SHADER_STAGE_VERTEX_BIT;
    cameraPushConstant.offset        = 0;
    cameraPushConstant.size          = sizeof(glm::mat4);

    Pipeline::PipelineInfo pipeInfo{};
    pipeInfo.vertexPath         = "/Shaders/GeometryVert.spv";
    pipeInfo.fragmentPath       = "/Shaders/GeometryFrag.spv";
    pipeInfo.colorFormats       = { m_Swapchain.GetFormat() };
    pipeInfo.depthFormat        = VK_FORMAT_D32_SFLOAT;
    pipeInfo.extent             = m_Swapchain.GetExtent();
    pipeInfo.layouts            = { m_ActiveScene->GetCamera().GetCameraLayout(), m_ActiveScene->GetSceneMembers()[0]->GetMesh()->m_SubMeshes[0].GetMaterial()->GetLayout() };
    pipeInfo.pushConstants      = { cameraPushConstant };

    m_GeometryPipeline = std::make_unique<Pipeline>(m_Context, pipeInfo);
}

void Renderer::CreateGeometryPassResources()
{
    Image::ImageInfo depthImage{};
    depthImage.format           = VK_FORMAT_D32_SFLOAT;
    depthImage.initialLayout    = VK_IMAGE_LAYOUT_UNDEFINED;
    depthImage.dimension        = m_Swapchain.GetExtent();
    depthImage.usageFlags       = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
    depthImage.aspectFlags      = VK_IMAGE_ASPECT_DEPTH_BIT;

    m_GeometryDepthImage = std::make_unique<Image>(m_Context, depthImage);
    m_GeometryDepthImage->ChangeLayout(VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL);
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
    m_Swapchain.ChangeLayout(m_ImageIndex, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR, VK_IMAGE_ASPECT_COLOR_BIT, m_CommandBuffers[m_FrameIndex]);
    VK_CHECK(vkEndCommandBuffer(m_CommandBuffers[m_FrameIndex]))

    VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT  };

    VkSubmitInfo submitInfo{};
    submitInfo.sType                = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.waitSemaphoreCount   = 1;
    submitInfo.pWaitSemaphores      = &m_ImageAvailableSems[m_FrameIndex];
    submitInfo.pWaitDstStageMask    = waitStages;
    submitInfo.commandBufferCount   = 1;
    submitInfo.pCommandBuffers      = &m_CommandBuffers[m_FrameIndex];
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores    = &m_PresentSems[m_FrameIndex];

    VK_CHECK(vkQueueSubmit(m_Context->GetGraphicsQueue(), 1, &submitInfo, m_InFlightFences[m_FrameIndex]))

    VkSwapchainKHR swapchain[] = {m_Swapchain.GetSwapchain()};

    VkPresentInfoKHR presentInfo{};
    presentInfo.sType               = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    presentInfo.waitSemaphoreCount  = 1;
    presentInfo.pWaitSemaphores     = &m_PresentSems[m_FrameIndex];
    presentInfo.swapchainCount      = 1;
    presentInfo.pSwapchains         = swapchain;
    presentInfo.pImageIndices       = &m_ImageIndex;

    VK_CHECK(vkQueuePresentKHR(m_Context->GetPresentQueue(), &presentInfo))
    m_FrameIndex = (m_FrameIndex + 1) % Swapchain::FRAMES_IN_FLIGHT;
}

/*
 * One Pass, Forward Render
 * Rendering directly into swapchain images
 *
 * Deferred renderer will render into separate buffers later
 */
void Renderer::GeometryPass()
{
    m_Swapchain.ChangeLayout(m_ImageIndex, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, VK_IMAGE_ASPECT_COLOR_BIT, m_CommandBuffers[m_FrameIndex]);

    Pipeline::Attachment colorAttachment{};
    colorAttachment.imageView   = m_Swapchain.GetImageViews()[m_ImageIndex];
    colorAttachment.imageLayout = m_Swapchain.GetImageLayouts()[m_ImageIndex];
    colorAttachment.loadOp      = VK_ATTACHMENT_LOAD_OP_CLEAR;
    colorAttachment.storeOp     = VK_ATTACHMENT_STORE_OP_STORE;
    colorAttachment.clearValue  = {};

    Pipeline::Attachment depthAttachment{};
    depthAttachment.imageView   = m_GeometryDepthImage->GetImageView();
    depthAttachment.imageLayout = m_GeometryDepthImage->GetImageLayout();
    depthAttachment.loadOp      = VK_ATTACHMENT_LOAD_OP_CLEAR;
    depthAttachment.storeOp     = VK_ATTACHMENT_STORE_OP_STORE;
    depthAttachment.clearValue  = {.depthStencil{1.0f, 0}};

    Pipeline::RenderInfo renderInfo{};
    renderInfo.colorAttachments = { colorAttachment };
    renderInfo.depthAttachment  = depthAttachment;
    renderInfo.extent           = m_Swapchain.GetExtent();

    m_GeometryPipeline->BeginRender(m_CommandBuffers[m_FrameIndex], renderInfo);

    m_GeometryPipeline->BindDescriptorSet(m_ActiveScene->GetCamera().GetDescriptorSet(m_FrameIndex), 0U);

    static auto startTime = std::chrono::high_resolution_clock::now();
    auto currentTime = std::chrono::high_resolution_clock::now();
    float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();

    for(const auto& sceneMember : m_ActiveScene->GetSceneMembers())
    {
        sceneMember->Rotate(0.0f, time * glm::radians(90.0f), 0.0f).Scale(1.0f, 1.0f, 1.0f)
                    .Translate(0.0f, 0.0f, -40.0f);
        sceneMember->UpdateModelMatrix();

        for(const auto& submesh : sceneMember->GetMesh()->m_SubMeshes)
        {
            m_GeometryPipeline->PushConstant(VK_SHADER_STAGE_VERTEX_BIT, 0U, sizeof(glm::mat4), &sceneMember->GetModelMatrix());
            m_GeometryPipeline->BindDescriptorSet(submesh.GetMaterial()->GetDescriptorSet(), 1U);
            m_GeometryPipeline->BindVertexBuffer(submesh.GetVertexBuffer());
            m_GeometryPipeline->BindIndexBuffer(submesh.GetIndexBuffer());
            m_GeometryPipeline->DrawIndexed(submesh.GetIndexBuffer().GetIndicesCount());
        }
    }

    m_GeometryPipeline->EndRender();
}

void Renderer::DrawFrame()
{
    BeginFrame();
    GeometryPass();
    EndFrame();
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