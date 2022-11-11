#pragma once

class Window;

class Context
{
public:
    enum class CommandType
    {
        GRAPHICS,
        TRANSFER
    };
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
    inline VkCommandPool GetCommandPool() const { return m_GraphicsCommandPool; }
    inline VkQueue GetGraphicsQueue() const { return m_GraphicsQueue; }
    inline VkQueue GetPresentQueue() const { return m_PresentQueue; }
    inline VkQueue GetTransferQueue() const { return m_TransferQueue; }
    inline VmaAllocator GetAllocator() const { return m_Allocator; }
public:
    VkCommandBuffer BeginSingleTimeCommands(CommandType type);
    void EndSingleTimeCommands(CommandType type, VkCommandBuffer commandBuffer);
private:
    void InitVulkan(const Window* window);
    void InitCommandPool();
    void InitTransferCommandPool();
private:
    VkInstance                  m_Instance;
    VmaAllocator                m_Allocator;
    VkDebugUtilsMessengerEXT    m_DebugMessenger;
    VkPhysicalDevice            m_PhysicalDevice;
    VkDevice                    m_LogicalDevice;
    VkQueue	                    m_GraphicsQueue;
    VkQueue	                    m_PresentQueue;
    VkQueue	                    m_TransferQueue;
    uint32_t                    m_GraphicsQueueFamily;
    uint32_t                    m_TransferQueueFamily;
    VkCommandPool               m_GraphicsCommandPool;
    VkCommandPool               m_TransferCommandPool;
    VkSurfaceKHR                m_Surface;
    VkExtent2D                  m_SurfaceExtent;
private:
    bool                        m_EnableValidation;
};
