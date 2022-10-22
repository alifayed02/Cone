#pragma once

class Window;

class Context
{
public:
    explicit Context(const Window* window);
    ~Context();

    Context(const Context& otherContext) = delete;
    Context& operator=(const Context& otherContext) = delete;
public:
    inline VkDevice GetLogicalDevice() const { return m_LogicalDevice; }
    inline VkPhysicalDevice GetPhysicalDevice() const { return m_PhysicalDevice; }
    inline VkSurfaceKHR GetSurface() const { return m_Surface; }
    inline VkExtent2D GetSurfaceExtent() const { return m_SurfaceExtent; }
    inline VkCommandPool GetCommandPool() const { return m_CommandPool; }
    inline VkQueue GetGraphicsQueue() const { return m_GraphicsQueue; }
    inline VkQueue GetPresentQueue() const { return m_PresentQueue; }
public:
    VkCommandBuffer BeginSingleTimeCommands();
    void EndSingleTimeCommands(VkCommandBuffer commandBuffer);
private:
    void InitVulkan(const Window* window);
    void InitCommandPool();
private:
    VkInstance                  m_Instance;
    VkDebugUtilsMessengerEXT    m_DebugMessenger;
    VkPhysicalDevice            m_PhysicalDevice;
    VkDevice                    m_LogicalDevice;
    VkQueue	                    m_GraphicsQueue;
    VkQueue	                    m_PresentQueue;
    uint32_t                    m_GraphicsQueueFamily;
    VkCommandPool               m_CommandPool;
    VkSurfaceKHR                m_Surface;
    VkExtent2D                  m_SurfaceExtent;
private:
    bool                        m_EnableValidation;
};
