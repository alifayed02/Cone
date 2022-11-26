#include "Core/CnPch.hpp"
#include "Pipeline.hpp"

#include "Buffer/Vertex.hpp"

Pipeline::Pipeline(Context* context, const PipelineInfo& info)
    :   m_Context(context), m_Pipeline{}, m_PipelineLayout{}, m_CurrentCommandBuffer{},
        m_DepthEnabled{info.depthFormat != VK_FORMAT_UNDEFINED}
{
    auto vertexCode     = ReadShaderCode(info.vertexPath);
    auto fragmentCode   = ReadShaderCode(info.fragmentPath);

    VkShaderModule vertexModule     = CreateShaderModule(vertexCode);
    VkShaderModule fragmentModule   = CreateShaderModule(fragmentCode);

    VkPipelineShaderStageCreateInfo vertexShaderStageInfo{};
    vertexShaderStageInfo.sType     = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    vertexShaderStageInfo.stage     = VK_SHADER_STAGE_VERTEX_BIT;
    vertexShaderStageInfo.module    = vertexModule;
    vertexShaderStageInfo.pName     = "main";

    VkPipelineShaderStageCreateInfo fragmentShaderStageInfo{};
    fragmentShaderStageInfo.sType   = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    fragmentShaderStageInfo.stage   = VK_SHADER_STAGE_FRAGMENT_BIT;
    fragmentShaderStageInfo.module  = fragmentModule;
    fragmentShaderStageInfo.pName   = "main";

    std::array<VkPipelineShaderStageCreateInfo, 2> shaderStages = { vertexShaderStageInfo, fragmentShaderStageInfo };

    auto vertexBindingDesc  = Vertex::GetBindingDescription();
    auto vertexAttribDesc   = Vertex::GetAttributeDescriptions();

    VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
    vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;

    if(info.vertexBindings)
    {
        vertexInputInfo.vertexBindingDescriptionCount   = 1;
        vertexInputInfo.pVertexBindingDescriptions      = &vertexBindingDesc;
        vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(vertexAttribDesc.size());
        vertexInputInfo.pVertexAttributeDescriptions    = vertexAttribDesc.data();
    }

    VkPipelineInputAssemblyStateCreateInfo inputAssemblyInfo{};
    inputAssemblyInfo.sType                     = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    inputAssemblyInfo.topology                  = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    inputAssemblyInfo.primitiveRestartEnable    = VK_FALSE;

    VkViewport viewport{};
    viewport.x          = 0.0f;
    viewport.y          = 0.0f;
    viewport.width      = static_cast<float>(info.extent.width);
    viewport.height     = static_cast<float>(info.extent.height);
    viewport.minDepth   = 0.0f;
    viewport.maxDepth   = 1.0f;

    VkRect2D scissor{};
    scissor.offset = {0, 0};
    scissor.extent = info.extent;

    VkPipelineViewportStateCreateInfo viewportInfo{};
    viewportInfo.sType          = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewportInfo.viewportCount  = 1;
    viewportInfo.pViewports     = &viewport;
    viewportInfo.scissorCount   = 1;
    viewportInfo.pScissors      = &scissor;

    VkPipelineRasterizationStateCreateInfo rasterizationInfo{};
    rasterizationInfo.sType                     = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rasterizationInfo.depthClampEnable          = VK_FALSE;
    rasterizationInfo.rasterizerDiscardEnable   = VK_FALSE;
    rasterizationInfo.polygonMode               = VK_POLYGON_MODE_FILL;
    rasterizationInfo.lineWidth                 = 1.0f;
    rasterizationInfo.cullMode                  = info.cullMode;
    rasterizationInfo.frontFace                 = VK_FRONT_FACE_COUNTER_CLOCKWISE;
    rasterizationInfo.depthBiasEnable           = VK_FALSE;

    VkPipelineMultisampleStateCreateInfo multisampleInfo{};
    multisampleInfo.sType                   = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    multisampleInfo.sampleShadingEnable     = VK_FALSE;
    multisampleInfo.rasterizationSamples    = VK_SAMPLE_COUNT_1_BIT;

    VkPipelineDepthStencilStateCreateInfo depthStencilInfo{};
    depthStencilInfo.sType                  = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
    depthStencilInfo.depthTestEnable        = info.depthTest;
    depthStencilInfo.depthWriteEnable       = info.depthWrite;
    depthStencilInfo.depthCompareOp         = VK_COMPARE_OP_LESS;
    depthStencilInfo.depthBoundsTestEnable  = VK_FALSE;
    depthStencilInfo.minDepthBounds         = 0.0f; // Optional
    depthStencilInfo.maxDepthBounds         = 1.0f; // Optional
    depthStencilInfo.stencilTestEnable      = VK_FALSE;
    depthStencilInfo.front                  = {}; // Optional
    depthStencilInfo.back                   = {}; // Optional

    VkPipelineColorBlendAttachmentState colorBlendAttachment{};
    colorBlendAttachment.colorWriteMask         = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
    colorBlendAttachment.blendEnable            = info.enableBlend;
    colorBlendAttachment.srcColorBlendFactor    = VK_BLEND_FACTOR_SRC_ALPHA;
    colorBlendAttachment.dstColorBlendFactor    = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
    colorBlendAttachment.colorBlendOp           = VK_BLEND_OP_ADD;
    colorBlendAttachment.srcAlphaBlendFactor    = VK_BLEND_FACTOR_ONE;
    colorBlendAttachment.dstAlphaBlendFactor    = VK_BLEND_FACTOR_ZERO;
    colorBlendAttachment.alphaBlendOp           = VK_BLEND_OP_ADD;

    std::vector<VkPipelineColorBlendAttachmentState> colorBlendAttachments;
    for(size_t i = 0; i < info.colorFormats.size(); i++)
    {
        colorBlendAttachments.push_back(colorBlendAttachment);
    }

    VkPipelineColorBlendStateCreateInfo colorBlendInfo{};
    colorBlendInfo.sType                = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    colorBlendInfo.logicOpEnable        = VK_FALSE;
    colorBlendInfo.logicOp              = VK_LOGIC_OP_COPY;   // Optional
    colorBlendInfo.attachmentCount      = colorBlendAttachments.size();
    colorBlendInfo.pAttachments         = colorBlendAttachments.data();
    colorBlendInfo.blendConstants[0]    = 0.0f;     // Optional
    colorBlendInfo.blendConstants[1]    = 0.0f;     // Optional
    colorBlendInfo.blendConstants[2]    = 0.0f;     // Optional
    colorBlendInfo.blendConstants[3]    = 0.0f;     // Optional

    std::array<VkDynamicState, 2> dynamicStateEnables = { VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR };
    VkPipelineDynamicStateCreateInfo dynamicStateInfo{};
    dynamicStateInfo.sType              = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    dynamicStateInfo.dynamicStateCount  = static_cast<uint32_t>(dynamicStateEnables.size());
    dynamicStateInfo.pDynamicStates     = dynamicStateEnables.data();

    VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
    pipelineLayoutInfo.sType                    = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutInfo.setLayoutCount           = info.layouts.size();
    pipelineLayoutInfo.pSetLayouts              = info.layouts.data();
    pipelineLayoutInfo.pushConstantRangeCount   = info.pushConstants.size();
    pipelineLayoutInfo.pPushConstantRanges      = info.pushConstants.data();

    VK_CHECK(vkCreatePipelineLayout(m_Context->GetLogicalDevice(), &pipelineLayoutInfo, nullptr, &m_PipelineLayout))

    VkPipelineRenderingCreateInfo pipelineRenderingInfo{};
    pipelineRenderingInfo.sType                     = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO;
    pipelineRenderingInfo.colorAttachmentCount      = info.colorFormats.size();
    pipelineRenderingInfo.pColorAttachmentFormats   = info.colorFormats.data();
    pipelineRenderingInfo.depthAttachmentFormat     = info.depthFormat;

    VkGraphicsPipelineCreateInfo pipelineInfo{};
    pipelineInfo.sType                  = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pipelineInfo.pNext                  = &pipelineRenderingInfo;
    pipelineInfo.stageCount             = static_cast<uint32_t>(shaderStages.size());
    pipelineInfo.pStages                = shaderStages.data();
    pipelineInfo.pVertexInputState      = &vertexInputInfo;
    pipelineInfo.pInputAssemblyState    = &inputAssemblyInfo;
    pipelineInfo.pViewportState         = &viewportInfo;
    pipelineInfo.pRasterizationState    = &rasterizationInfo;
    pipelineInfo.pMultisampleState      = &multisampleInfo;
    pipelineInfo.pDepthStencilState     = &depthStencilInfo; // Optional
    pipelineInfo.pColorBlendState       = &colorBlendInfo;
    pipelineInfo.pDynamicState          = &dynamicStateInfo;
    pipelineInfo.layout                 = m_PipelineLayout;
    pipelineInfo.renderPass             = VK_NULL_HANDLE;
    pipelineInfo.subpass                = 0U;

    VK_CHECK(vkCreateGraphicsPipelines(m_Context->GetLogicalDevice(), VK_NULL_HANDLE, 1U, &pipelineInfo, nullptr, &m_Pipeline))

    vkDestroyShaderModule(m_Context->GetLogicalDevice(), vertexModule, nullptr);
    vkDestroyShaderModule(m_Context->GetLogicalDevice(), fragmentModule, nullptr);
}

void Pipeline::BeginRender(VkCommandBuffer commandBuffer, const RenderInfo& renderInfo)
{
    m_CurrentCommandBuffer = commandBuffer;
    std::vector<VkRenderingAttachmentInfo> renderAttachments;

    for(size_t i = 0; i < renderInfo.colorAttachments.size(); i++)
    {
        const Attachment& attachment = renderInfo.colorAttachments[i];

        VkRenderingAttachmentInfo colorRenderingAttachmentInfo{};
        colorRenderingAttachmentInfo.sType       = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO;
        colorRenderingAttachmentInfo.imageView   = attachment.imageView;
        colorRenderingAttachmentInfo.imageLayout = attachment.imageLayout;
        colorRenderingAttachmentInfo.loadOp      = attachment.loadOp;
        colorRenderingAttachmentInfo.storeOp     = attachment.storeOp;
        colorRenderingAttachmentInfo.clearValue  = attachment.clearValue;

        renderAttachments.push_back(colorRenderingAttachmentInfo);
    }

    VkRenderingAttachmentInfo depthRenderingAttachmentInfo{};
    if(m_DepthEnabled)
    {
        depthRenderingAttachmentInfo.sType          = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO;
        depthRenderingAttachmentInfo.imageView      = renderInfo.depthAttachment.imageView;
        depthRenderingAttachmentInfo.imageLayout    = renderInfo.depthAttachment.imageLayout;
        depthRenderingAttachmentInfo.loadOp         = renderInfo.depthAttachment.loadOp;
        depthRenderingAttachmentInfo.storeOp        = renderInfo.depthAttachment.storeOp;
        depthRenderingAttachmentInfo.clearValue     = renderInfo.depthAttachment.clearValue;
    }

    VkRenderingInfo dynRenderingInfo{};
    dynRenderingInfo.sType                  = VK_STRUCTURE_TYPE_RENDERING_INFO;
    dynRenderingInfo.renderArea             = {{0U, 0U}, renderInfo.extent};
    dynRenderingInfo.layerCount             = 1U;
    dynRenderingInfo.colorAttachmentCount   = static_cast<uint32_t>(renderAttachments.size());
    dynRenderingInfo.pColorAttachments      = renderAttachments.data();
    dynRenderingInfo.pDepthAttachment       = m_DepthEnabled ? &depthRenderingAttachmentInfo : nullptr;

    vkCmdBeginRenderingKHR(m_CurrentCommandBuffer, &dynRenderingInfo);

    VkViewport viewport{};
    viewport.width      = static_cast<float>(renderInfo.extent.width);
    viewport.height     = static_cast<float>(renderInfo.extent.height);
    viewport.maxDepth   = 1.0f;

    VkRect2D scissor{};
    scissor.extent = renderInfo.extent;

    vkCmdBindPipeline(m_CurrentCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_Pipeline);

    vkCmdSetViewport(m_CurrentCommandBuffer, 0U, 1U, &viewport);
    vkCmdSetScissor(m_CurrentCommandBuffer, 0U, 1U, &scissor);
}

void Pipeline::EndRender()
{
    vkCmdEndRenderingKHR(m_CurrentCommandBuffer);
    m_CurrentCommandBuffer = VK_NULL_HANDLE;
}

void Pipeline::Draw(uint32_t vertexCount)
{
    vkCmdDraw(m_CurrentCommandBuffer, vertexCount, 1, 0, 0);
}

void Pipeline::DrawIndexed(const uint32_t indexCount)
{
    vkCmdDrawIndexed(m_CurrentCommandBuffer, indexCount, 1, 0, 0, 0);
}

void Pipeline::BindVertexBuffer(const VertexBuffer& vb)
{
    vb.Bind(m_CurrentCommandBuffer);
}

void Pipeline::BindIndexBuffer(const IndexBuffer& ib)
{
    ib.Bind(m_CurrentCommandBuffer);
}

void Pipeline::BindDescriptorSet(VkDescriptorSet descriptorSet, const uint32_t index)
{
    vkCmdBindDescriptorSets(m_CurrentCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_PipelineLayout, index, 1U, &descriptorSet, 0U, nullptr);
}

void Pipeline::PushConstant(VkShaderStageFlags shaderStageFlags, uint32_t offset, uint32_t size, const void* data)
{
    vkCmdPushConstants(m_CurrentCommandBuffer, m_PipelineLayout, shaderStageFlags, offset, size, data);
}

std::vector<char> Pipeline::ReadShaderCode(std::string_view path)
{
    std::filesystem::path cwd = std::filesystem::current_path().parent_path();
    std::string fullPath = cwd.string() + std::string(path);
    std::ifstream shaderFile(fullPath, std::ios::ate | std::ios::binary);

    if(!shaderFile.is_open())
    {
        throw std::runtime_error("Error: Failed to open file " + fullPath);
    }

    std::streamsize fileSize = static_cast<std::streamsize>(shaderFile.tellg());
    std::vector<char> buffer(fileSize);

    shaderFile.seekg(0);
    shaderFile.read(buffer.data(), fileSize);
    shaderFile.close();

    return buffer;
}

VkShaderModule Pipeline::CreateShaderModule(std::span<char> shaderCode)
{
    VkShaderModuleCreateInfo moduleCreateInfo{};
    moduleCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    moduleCreateInfo.codeSize = shaderCode.size();
    moduleCreateInfo.pCode = reinterpret_cast<const uint32_t*>(shaderCode.data());

    VkShaderModule shaderModule;

    VK_CHECK(vkCreateShaderModule(m_Context->GetLogicalDevice(), &moduleCreateInfo, nullptr, &shaderModule))

    return shaderModule;
}

Pipeline::~Pipeline()
{
    if(m_PipelineLayout)
    {
        vkDestroyPipelineLayout(m_Context->GetLogicalDevice(), m_PipelineLayout, nullptr);
    }

    if(m_Pipeline)
    {
        vkDestroyPipeline(m_Context->GetLogicalDevice(), m_Pipeline, nullptr);
    }
}