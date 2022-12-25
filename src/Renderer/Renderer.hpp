#pragma once

#include "Swapchain.hpp"
#include "Pipeline.hpp"
#include "Framebuffer.hpp"
#include "DescriptorSet.hpp"
#include "Buffer/VertexBuffer.hpp"
#include "Buffer/IndexBuffer.hpp"
#include "Scene/Lighting/Lights.hpp"
#include "Scene/PostProcessing/Tonemapping.hpp"

class Context;
class Scene;

class Renderer
{
public:
    struct DirectionalShadowMap
    {
        std::unique_ptr<Image>                          perLightImage;
        VkImageView                                     sharedLightView;
        std::vector<Lights::DirectionalShadowBuffer>    data;
    };
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
    void CreateShadowPassResources();
    void CreateShadowPipeline();
    void UpdateShadowProjections();
private:
    void CreateGeometryPassResources();
    void CreateGeometryPipeline();
private:
    void CreateLightObjects();
    void CreateLightBuffers();
    void UpdateLights();
    void CreateLightingPassResources();
    void CreateLightingPipeline();
private:
    void CreateTonemappingPassResources();
    void CreateTonemappingPipeline();
private:
    void BeginFrame();
    void EndFrame();
    void ShadowPass();
    void GeometryPass();
    void LightingPass();
    void TonemappingPass();
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
    // Shadow Pass Resources
    std::unique_ptr<Pipeline>                                           m_ShadowPipeline;
    std::array<DirectionalShadowMap, Swapchain::FRAMES_IN_FLIGHT>       m_DirectionalMaps;
    VkSampler                                                           m_ShadowMapSampler;
private:
    // Geometry Pass Resources
    std::unique_ptr<Pipeline>                                               m_GeometryPipeline;
    std::array<std::unique_ptr<Framebuffer>, Swapchain::FRAMES_IN_FLIGHT>   m_GeometryBuffer;
private:
    // Lighting Pass Resources
    std::unique_ptr<Pipeline>                                               m_LightingPipeline;
    std::array<std::unique_ptr<DescriptorSet>, Swapchain::FRAMES_IN_FLIGHT> m_GBufferDescriptorSets;
    std::array<std::unique_ptr<Image>, Swapchain::FRAMES_IN_FLIGHT>         m_HDRImages;
    std::array<Lights::LightBufferObject, Swapchain::FRAMES_IN_FLIGHT>      m_LightsObjects;
    std::array<std::unique_ptr<Buffer>, Swapchain::FRAMES_IN_FLIGHT>        m_LightsBuffers;
private:
    // Tone Mapping Pass Resources
    std::unique_ptr<Pipeline>                                                   m_TonemappingPipeline;
    std::array<PostProcessing::TonemappingParams, Swapchain::FRAMES_IN_FLIGHT>  m_TonemappingParams;
    std::array<std::unique_ptr<DescriptorSet>, Swapchain::FRAMES_IN_FLIGHT>     m_HDRDescriptorSets;
};
