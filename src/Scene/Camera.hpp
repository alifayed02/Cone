#pragma once

#include "Renderer/Swapchain.hpp"
#include "Renderer/Buffer/Buffer.hpp"

#include "glm/glm.hpp"

class Context;

class Camera
{
public:
    struct CameraBufferObject
    {
        glm::mat4 viewMatrix;
        glm::mat4 projectionMatrix;
        glm::mat4 viewProjectionMatrix;
    };
public:
    explicit Camera(Context* context);
    ~Camera();
public:
    void WriteBuffer(uint32_t frameIndex);
    void Bind(VkCommandBuffer commandBuffer, VkPipelineLayout pipelineLayout, uint32_t frameIndex, uint32_t setIndex) const;
    void SetExtent(VkExtent2D extent);
public:
    inline VkDescriptorSetLayout GetCameraLayout() const { return m_DescriptorSetLayout; }
    inline auto& GetBufferObjects() { return m_BufferObjects; }
private:
    void CreateDescriptorPool();
    void CreateDescriptorSet();
    void CreateDescriptorBuffers();
private:
    Context*                                                            m_Context;
    VkExtent2D                                                          m_CameraExtent;
    VkDescriptorPool                                                    m_DescriptorPool;
    VkDescriptorSetLayout                                               m_DescriptorSetLayout;
    std::array<VkDescriptorSet, Swapchain::FRAMES_IN_FLIGHT>            m_DescriptorSets;
    std::array<std::unique_ptr<Buffer>, Swapchain::FRAMES_IN_FLIGHT>    m_Buffers;
    std::array<CameraBufferObject, Swapchain::FRAMES_IN_FLIGHT>         m_BufferObjects;
};
