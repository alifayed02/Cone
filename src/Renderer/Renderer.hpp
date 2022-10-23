#pragma once

#include "Swapchain.hpp"
#include "Pipeline.hpp"

#include "Buffer/VertexBuffer.hpp"
#include "Buffer/IndexBuffer.hpp"

class Context;

class Renderer
{
public:
    Renderer(Context* context);
    ~Renderer();

    Renderer(const Renderer& otherRenderer) = delete;
    Renderer& operator=(const Renderer& otherRenderer) = delete;
public:
    void DrawFrame(const VertexBuffer& vb, const IndexBuffer& ib);
private:
    void Init();
    void CreateCommandBuffers();
    void CreateSyncResources();
    void CreateGeometryPipeline();
    void BeginFrame();
    void EndFrame();
    void GeometryPass(const VertexBuffer& vb, const IndexBuffer& ib);
private:
    Context*                                                    m_Context;
    Swapchain                                                   m_Swapchain;
    std::unique_ptr<Pipeline>                                   m_GeometryPipeline;
    std::array<VkCommandBuffer, Swapchain::FRAMES_IN_FLIGHT>    m_CommandBuffers;
    std::array<VkFence, Swapchain::FRAMES_IN_FLIGHT>            m_InFlightFences;
    std::array<VkSemaphore, Swapchain::FRAMES_IN_FLIGHT>        m_ImageAvailableSems;
    std::array<VkSemaphore, Swapchain::FRAMES_IN_FLIGHT>        m_PresentSems;
    uint32_t                                                    m_ImageIndex;
    size_t                                                      m_FrameIndex;
};
