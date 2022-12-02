#pragma once

#include "Swapchain.hpp"
#include "Pipeline.hpp"
#include "Framebuffer.hpp"
#include "DescriptorSet.hpp"
#include "Buffer/VertexBuffer.hpp"
#include "Buffer/IndexBuffer.hpp"
#include "Scene/Lights.hpp"

class Context;
class Scene;

class Renderer
{
public:
    Renderer(Context* context, Scene* scene);
    ~Renderer();

    Renderer(const Renderer& otherRenderer) = delete;
    Renderer& operator=(const Renderer& otherRenderer) = delete;
public:
    void DrawFrame();
public:
    inline void SetActiveScene(Scene* scene) { m_ActiveScene = scene; }
    inline uint32_t GetCurrentFrame() const { return m_FrameIndex; }
private:
    void Init();
    void CreateCommandBuffers();
    void CreateSyncResources();
private:
    void CreateGeometryPassResources();
    void CreateGeometryPipeline();
private:
    void CreateLightObjects();
    void CreateLightBuffers();
    void CreateLightingPassResources();
    void CreateLightingPipeline();
private:
    void BeginFrame();
    void EndFrame();
    void GeometryPass();
    void LightingPass();
private:
    Context*                                                    m_Context;
    Scene*                                                      m_ActiveScene;
    Swapchain                                                   m_Swapchain;
    std::array<VkCommandBuffer, Swapchain::FRAMES_IN_FLIGHT>    m_CommandBuffers;
    std::array<VkFence, Swapchain::FRAMES_IN_FLIGHT>            m_InFlightFences;
    std::array<VkSemaphore, Swapchain::FRAMES_IN_FLIGHT>        m_ImageAvailableSems;
    std::array<VkSemaphore, Swapchain::FRAMES_IN_FLIGHT>        m_PresentSems;
    uint32_t                                                    m_ImageIndex;
    size_t                                                      m_FrameIndex;
private:
    // Geometry Pass Resources
    std::unique_ptr<Pipeline>                                               m_GeometryPipeline;
    std::array<std::unique_ptr<Framebuffer>, Swapchain::FRAMES_IN_FLIGHT>   m_GeometryBuffer;
private:
    // Lighting Pass Resources
    std::unique_ptr<Pipeline>                                               m_LightingPipeline;
    VkDescriptorPool                                                        m_GBufferDescriptorPool;
    VkDescriptorSetLayout                                                   m_GBufferDescriptorSetLayout;
    std::array<VkDescriptorSet, Swapchain::FRAMES_IN_FLIGHT>                m_GBufferDescriptorSets;
    std::array<std::unique_ptr<DescriptorSet>, Swapchain::FRAMES_IN_FLIGHT> m_NewGBufferDescriptorSets;
    std::array<Lights::LightBufferObject, Swapchain::FRAMES_IN_FLIGHT>      m_LightsObjects;
    std::array<std::unique_ptr<Buffer>, Swapchain::FRAMES_IN_FLIGHT>        m_LightsBuffers;
};
