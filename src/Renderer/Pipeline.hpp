#pragma once

#include "Context.hpp"

class Pipeline
{
public:
    struct PipelineInfo
    {
        std::string_view        vertexPath;
        std::string_view        fragmentPath;
        std::vector<VkFormat>   colorFormats;
        VkExtent2D              extent;
    };
    struct Attachment
    {
        VkImageView         imageView{};
        VkImageLayout       imageLayout{};
        VkAttachmentLoadOp  loadOp{};
        VkAttachmentStoreOp storeOp{};
        VkClearValue        clearValue{};
    };
    struct RenderInfo
    {
        std::vector<Attachment> colorAttachments;
        VkExtent2D              extent;
    };
public:
    Pipeline(Context* context, const PipelineInfo& info);
    ~Pipeline();

    Pipeline(const Pipeline& otherPipeline) = delete;
    Pipeline& operator=(const Pipeline& otherPipeline) = delete;
public:
    void BeginRender(VkCommandBuffer commandBuffer, const RenderInfo& renderInfo);
    void EndRender();
    void Draw(const uint32_t vertexCount);
private:
    std::vector<char> ReadShaderCode(std::string_view path);
    VkShaderModule CreateShaderModule(std::span<char> shaderCode);
private:
    Context*            m_Context;
    VkPipeline          m_Pipeline;
    VkPipelineLayout    m_PipelineLayout;
    VkCommandBuffer     m_CurrentCommandBuffer;
};
