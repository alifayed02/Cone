#pragma once

#include "Swapchain.hpp"
#include "Pipeline.hpp"
#include "Framebuffer.hpp"

#include "Buffer/VertexBuffer.hpp"
#include "Buffer/IndexBuffer.hpp"

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
private:
    void Init();
    void CreateCommandBuffers();
    void CreateSyncResources();
    void CreateGeometryPipeline();
    void CreateGeometryPassResources();
private:
    void BeginFrame();
    void EndFrame();
    void GeometryPass();
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
    std::unique_ptr<Pipeline>       m_GeometryPipeline;
    std::unique_ptr<Image>          m_GeometryDepthImage;
};
