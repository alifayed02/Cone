#include "Core/CnPch.hpp"
#include "Renderer.hpp"

#include "Scene/Scene.hpp"
#include "Scene/SceneMember.hpp"
#include "Asset/Mesh.hpp"
#include "Asset/Material.hpp"
#include "Context.hpp"

#include "glm/gtc/matrix_transform.hpp"

Renderer::Renderer(Context* context, Scene* scene)
    :   m_Context{context}, m_ActiveScene{scene}, m_Swapchain{context}, m_CommandBuffers{},
        m_InFlightFences{}, m_ImageAvailableSems{}, m_PresentSems{}, m_ImageIndex{}, m_FrameIndex{},
        m_GBufferDescriptorPool{}, m_GBufferDescriptorSetLayout{}, m_GBufferDescriptorSets{}
{
    Init();
    m_ActiveScene->GetCamera().SetExtent(m_Swapchain.GetExtent());
}

void Renderer::Init()
{
    CreateCommandBuffers();
    CreateSyncResources();
    CreateGeometryPassResources();
    CreateGeometryPipeline();
    CreateLightingPassResources();
    CreateLightingPipeline();
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

void Renderer::CreateGeometryPassResources()
{
    // Create GBuffer
    for(size_t i = 0; i < m_GeometryBuffer.max_size(); i++)
    {
        std::vector<Framebuffer::AttachmentInfo> gBufferAttachments;
        gBufferAttachments.resize(4);

        // Albedo Attachment
        gBufferAttachments[0].format        = m_Swapchain.GetFormat();
        gBufferAttachments[0].layout        = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
        gBufferAttachments[0].usageFlags    = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
        gBufferAttachments[0].aspectFlags   = VK_IMAGE_ASPECT_COLOR_BIT;

        // Position Attachment
        gBufferAttachments[1].format        = VK_FORMAT_R16G16B16A16_SFLOAT;
        gBufferAttachments[1].layout        = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
        gBufferAttachments[1].usageFlags    = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
        gBufferAttachments[1].aspectFlags   = VK_IMAGE_ASPECT_COLOR_BIT;

        // Normal Attachment
        gBufferAttachments[2].format        = VK_FORMAT_R16G16B16A16_SFLOAT;
        gBufferAttachments[2].layout        = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
        gBufferAttachments[2].usageFlags    = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
        gBufferAttachments[2].aspectFlags   = VK_IMAGE_ASPECT_COLOR_BIT;

        // Depth Attachment
        gBufferAttachments[3].format        = VK_FORMAT_D32_SFLOAT;
        gBufferAttachments[3].layout        = VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL;
        gBufferAttachments[3].usageFlags    = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
        gBufferAttachments[3].aspectFlags   = VK_IMAGE_ASPECT_DEPTH_BIT;

        m_GeometryBuffer[i] = std::make_unique<Framebuffer>(m_Context, m_Swapchain.GetExtent(), gBufferAttachments);
    }
}

void Renderer::CreateGeometryPipeline()
{
    VkPushConstantRange cameraPushConstant{};
    cameraPushConstant.stageFlags    = VK_SHADER_STAGE_VERTEX_BIT;
    cameraPushConstant.offset        = 0;
    cameraPushConstant.size          = sizeof(glm::mat4);

    VkPushConstantRange materialPushConstant{};
    materialPushConstant.stageFlags    = VK_SHADER_STAGE_FRAGMENT_BIT;
    materialPushConstant.offset        = sizeof(glm::mat4);
    materialPushConstant.size          = sizeof(Material::MaterialObject);

    std::vector<VkFormat> colorFormats;
    for(size_t i = 0; i < m_GeometryBuffer[0]->GetAttachments().size() - 1; i++)
    {
        colorFormats.push_back(m_GeometryBuffer[0]->GetAttachments()[i].GetImageFormat());
    }
    VkFormat depthFormat = m_GeometryBuffer[0]->GetAttachments()[m_GeometryBuffer[0]->GetAttachments().size() - 1].GetImageFormat();

    Pipeline::PipelineInfo pipeInfo{};
    pipeInfo.vertexPath         = "/Shaders/GeometryVert.spv";
    pipeInfo.fragmentPath       = "/Shaders/GeometryFrag.spv";
    pipeInfo.colorFormats       = colorFormats;
    pipeInfo.depthFormat        = depthFormat;
    pipeInfo.extent             = m_Swapchain.GetExtent();
    pipeInfo.cullMode           = VK_CULL_MODE_BACK_BIT;
    pipeInfo.depthTest          = VK_TRUE;
    pipeInfo.depthWrite         = VK_TRUE;
    pipeInfo.vertexBindings     = VK_TRUE;
    pipeInfo.enableBlend        = VK_FALSE;
    pipeInfo.layouts            = { m_ActiveScene->GetCamera().GetCameraLayout(), m_ActiveScene->GetSceneMembers()[0].GetMesh()->m_SubMeshes[0].GetMaterial()->GetLayout() };
    pipeInfo.pushConstants      = { cameraPushConstant, materialPushConstant };

    m_GeometryPipeline = std::make_unique<Pipeline>(m_Context, pipeInfo);
}

void Renderer::CreateLightObjects()
{
    Lights::LightBufferObject lbo{};

    for(size_t i = 0; i < m_ActiveScene->GetPointLights().size(); i++)
    {
        lbo.pointlights[i] = m_ActiveScene->GetPointLights()[i];
        lbo.numPointLights++;
    }

    for(size_t i = 0; i < m_LightsObjects.max_size(); i++)
    {
        m_LightsObjects[i] = lbo;
    }
}

void Renderer::CreateLightBuffers()
{
    for(size_t i = 0; i < m_LightsBuffers.max_size(); i++)
    {
        Buffer::BufferInfo bufferInfo{};
        bufferInfo.size = sizeof(Lights::LightBufferObject);
        bufferInfo.usageFlags = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
        bufferInfo.vmaMemoryUsage = VMA_MEMORY_USAGE_AUTO;
        bufferInfo.vmaAllocFlags = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT | VMA_ALLOCATION_CREATE_MAPPED_BIT;

        m_LightsBuffers[i] = std::make_unique<Buffer>(m_Context, bufferInfo);
        m_LightsBuffers[i]->Map(&m_LightsObjects[i], sizeof(Lights::LightBufferObject));
    }
}

void Renderer::CreateLightingPassResources()
{
    CreateLightObjects();
    CreateLightBuffers();

    for(size_t i = 0; i < static_cast<uint32_t>(Swapchain::FRAMES_IN_FLIGHT); i++)
    {
        // Albedo
        VkDescriptorImageInfo albedoDescriptorInfo{};
        albedoDescriptorInfo.sampler       = m_GeometryBuffer[i]->GetSampler();
        albedoDescriptorInfo.imageView     = m_GeometryBuffer[i]->GetAttachments()[0].GetImageView();
        albedoDescriptorInfo.imageLayout   = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

        DescriptorSet::BindingInfo albedoSamplerBinding{};
        albedoSamplerBinding.type       = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        albedoSamplerBinding.binding    = 0;
        albedoSamplerBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
        albedoSamplerBinding.imageInfo  = &albedoDescriptorInfo;

        // Position
        VkDescriptorImageInfo positionDescriptorInfo{};
        positionDescriptorInfo.sampler       = m_GeometryBuffer[i]->GetSampler();
        positionDescriptorInfo.imageView     = m_GeometryBuffer[i]->GetAttachments()[1].GetImageView();
        positionDescriptorInfo.imageLayout   = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

        DescriptorSet::BindingInfo positionSamplerBinding{};
        positionSamplerBinding.type       = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        positionSamplerBinding.binding    = 1;
        positionSamplerBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
        positionSamplerBinding.imageInfo  = &positionDescriptorInfo;

        // Normal
        VkDescriptorImageInfo normalDescriptorInfo{};
        normalDescriptorInfo.sampler       = m_GeometryBuffer[i]->GetSampler();
        normalDescriptorInfo.imageView     = m_GeometryBuffer[i]->GetAttachments()[2].GetImageView();
        normalDescriptorInfo.imageLayout   = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

        DescriptorSet::BindingInfo normalSamplerBinding{};
        normalSamplerBinding.type       = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        normalSamplerBinding.binding    = 2;
        normalSamplerBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
        normalSamplerBinding.imageInfo  = &normalDescriptorInfo;

        // Lights
        VkDescriptorBufferInfo lightBufferInfo{};
        lightBufferInfo.buffer   = m_LightsBuffers[i]->GetBuffer();
        lightBufferInfo.offset   = 0;
        lightBufferInfo.range    = sizeof(Lights::LightBufferObject);

        DescriptorSet::BindingInfo lightBufferBinding{};
        lightBufferBinding.type         = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        lightBufferBinding.binding      = 3;
        lightBufferBinding.stageFlags   = VK_SHADER_STAGE_FRAGMENT_BIT;
        lightBufferBinding.bufferInfo   = &lightBufferInfo;

        std::vector<DescriptorSet::BindingInfo> bindings =
                {
                    albedoSamplerBinding,
                    positionSamplerBinding,
                    normalSamplerBinding,
                    lightBufferBinding
                };

        m_NewGBufferDescriptorSets[i] = std::make_unique<DescriptorSet>(m_Context, bindings);
    }
}

void Renderer::CreateLightingPipeline()
{
    // Full screen quad for now. Bounding spheres planned.
    Pipeline::PipelineInfo pipeInfo{};
    pipeInfo.vertexPath         = "/Shaders/FullScreenQuadVert.spv";
    pipeInfo.fragmentPath       = "/Shaders/LightingFrag.spv";
    pipeInfo.colorFormats       = { m_Swapchain.GetFormat() };
    pipeInfo.extent             = m_Swapchain.GetExtent();
    pipeInfo.cullMode           = VK_CULL_MODE_FRONT_BIT;
    pipeInfo.depthTest          = VK_FALSE;
    pipeInfo.depthWrite         = VK_FALSE;
    pipeInfo.vertexBindings     = VK_FALSE;
    pipeInfo.enableBlend        = VK_FALSE;
    pipeInfo.layouts            = { m_NewGBufferDescriptorSets[0]->GetDescriptorSetLayout() };

    m_LightingPipeline = std::make_unique<Pipeline>(m_Context, pipeInfo);
}

void Renderer::GeometryPass()
{
    // Change GBuffer Image Layouts to VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL
    for(size_t i = 0; i < m_GeometryBuffer[m_FrameIndex]->GetAttachments().size() - 1; i++)
    {
        m_GeometryBuffer[m_FrameIndex]->GetAttachments()[i].ChangeLayout(VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT);
    }

    std::vector<Pipeline::Attachment> colorAttachments;

    Pipeline::Attachment albedoAttachment{};
    albedoAttachment.imageView      = m_GeometryBuffer[m_FrameIndex]->GetAttachments()[0].GetImageView();
    albedoAttachment.imageLayout    = m_GeometryBuffer[m_FrameIndex]->GetAttachments()[0].GetImageLayout();
    albedoAttachment.loadOp         = VK_ATTACHMENT_LOAD_OP_CLEAR;
    albedoAttachment.storeOp        = VK_ATTACHMENT_STORE_OP_STORE;
    albedoAttachment.clearValue     = {};
    colorAttachments.push_back(albedoAttachment);

    Pipeline::Attachment positionAttachment{};
    positionAttachment.imageView      = m_GeometryBuffer[m_FrameIndex]->GetAttachments()[1].GetImageView();
    positionAttachment.imageLayout    = m_GeometryBuffer[m_FrameIndex]->GetAttachments()[1].GetImageLayout();
    positionAttachment.loadOp         = VK_ATTACHMENT_LOAD_OP_CLEAR;
    positionAttachment.storeOp        = VK_ATTACHMENT_STORE_OP_STORE;
    positionAttachment.clearValue     = {};
    colorAttachments.push_back(positionAttachment);

    Pipeline::Attachment normalAttachment{};
    normalAttachment.imageView      = m_GeometryBuffer[m_FrameIndex]->GetAttachments()[2].GetImageView();
    normalAttachment.imageLayout    = m_GeometryBuffer[m_FrameIndex]->GetAttachments()[2].GetImageLayout();
    normalAttachment.loadOp         = VK_ATTACHMENT_LOAD_OP_CLEAR;
    normalAttachment.storeOp        = VK_ATTACHMENT_STORE_OP_STORE;
    normalAttachment.clearValue     = {};
    colorAttachments.push_back(normalAttachment);

    Pipeline::Attachment depthAttachment{};
    depthAttachment.imageView   = m_GeometryBuffer[m_FrameIndex]->GetAttachments()[3].GetImageView();
    depthAttachment.imageLayout = m_GeometryBuffer[m_FrameIndex]->GetAttachments()[3].GetImageLayout();
    depthAttachment.loadOp      = VK_ATTACHMENT_LOAD_OP_CLEAR;
    depthAttachment.storeOp     = VK_ATTACHMENT_STORE_OP_STORE;
    depthAttachment.clearValue  = {.depthStencil{1.0f, 0}};

    Pipeline::RenderInfo renderInfo{};
    renderInfo.colorAttachments = colorAttachments;
    renderInfo.depthAttachment  = depthAttachment;
    renderInfo.extent           = m_Swapchain.GetExtent();

    m_GeometryPipeline->BeginRender(m_CommandBuffers[m_FrameIndex], renderInfo);

    m_GeometryPipeline->BindDescriptorSet(m_ActiveScene->GetCamera().GetDescriptorSet(m_FrameIndex), 0U);
    for(const auto& sceneMember : m_ActiveScene->GetSceneMembers())
    {
        m_GeometryPipeline->PushConstant(VK_SHADER_STAGE_VERTEX_BIT, 0U, sizeof(glm::mat4), &sceneMember.GetModelMatrix());
        for(const auto& submesh : sceneMember.GetMesh()->m_SubMeshes)
        {
            m_GeometryPipeline->PushConstant(VK_SHADER_STAGE_FRAGMENT_BIT, sizeof(glm::mat4), sizeof(submesh.GetMaterial()->GetMaterialObject()), &submesh.GetMaterial()->GetMaterialObject());
            m_GeometryPipeline->BindDescriptorSet(submesh.GetMaterial()->GetDescriptorSet(), 1U);
            m_GeometryPipeline->BindVertexBuffer(submesh.GetVertexBuffer());
            m_GeometryPipeline->BindIndexBuffer(submesh.GetIndexBuffer());
            m_GeometryPipeline->DrawIndexed(submesh.GetIndexBuffer().GetIndicesCount());
        }
    }

    m_GeometryPipeline->EndRender();
}

void Renderer::LightingPass()
{
    // Change Swapchain Image Layout
    m_Swapchain.ChangeLayout(m_ImageIndex, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, VK_IMAGE_ASPECT_COLOR_BIT, m_CommandBuffers[m_FrameIndex]);

    // Change GBuffer Image Layouts to VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
    for(size_t i = 0; i < m_GeometryBuffer[m_FrameIndex]->GetAttachments().size() - 1; i++)
    {
        m_GeometryBuffer[m_FrameIndex]->GetAttachments()[i].ChangeLayout(VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
    }

    Pipeline::Attachment colorAttachment{};
    colorAttachment.imageView   = m_Swapchain.GetImageViews()[m_ImageIndex];
    colorAttachment.imageLayout = m_Swapchain.GetImageLayouts()[m_ImageIndex];
    colorAttachment.loadOp      = VK_ATTACHMENT_LOAD_OP_CLEAR;
    colorAttachment.storeOp     = VK_ATTACHMENT_STORE_OP_STORE;
    colorAttachment.clearValue  = {};

    Pipeline::RenderInfo renderInfo{};
    renderInfo.colorAttachments = { colorAttachment };
    renderInfo.extent           = m_Swapchain.GetExtent();

    m_LightingPipeline->BeginRender(m_CommandBuffers[m_FrameIndex], renderInfo);
    m_LightingPipeline->BindDescriptorSet(m_NewGBufferDescriptorSets[m_FrameIndex]->GetDescriptorSet(), 0U);
    m_LightingPipeline->Draw(3);
    m_LightingPipeline->EndRender();
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

    VkSwapchainKHR swapchain[] = { m_Swapchain.GetSwapchain() };

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

void Renderer::DrawFrame()
{
    BeginFrame();
    GeometryPass();
    LightingPass();
    EndFrame();
}

Renderer::~Renderer()
{
    vkDeviceWaitIdle(m_Context->GetLogicalDevice());

    if(m_GBufferDescriptorSetLayout)
    {
        vkDestroyDescriptorSetLayout(m_Context->GetLogicalDevice(), m_GBufferDescriptorSetLayout, nullptr);
    }
    if(m_GBufferDescriptorPool)
    {
        vkDestroyDescriptorPool(m_Context->GetLogicalDevice(), m_GBufferDescriptorPool, nullptr);
    }
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