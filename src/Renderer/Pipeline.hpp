#pragma once

#include "Context.hpp"

class Pipeline
{
public:
    struct PipelineInfo
    {
        std::string_view vertexPath;
        std::string_view fragmentPath;

        std::vector<VkFormat> colorFormats;
    };
public:
    Pipeline(Context* context, const PipelineInfo& info);
    ~Pipeline();

    Pipeline(const Pipeline& otherPipeline) = delete;
    Pipeline& operator=(const Pipeline& otherPipeline) = delete;
private:
    std::vector<char> ReadShaderCode(std::string_view path);
    VkShaderModule CreateShaderModule(std::span<char> shaderCode);
private:
    Context*            m_Context;
    VkPipeline          m_Pipeline;
    VkPipelineLayout    m_PipelineLayout;
};
