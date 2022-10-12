#pragma once

#include "Core/CnPch.hpp"

class Window;

class Context
{
public:
    explicit Context(const Window* window);
    ~Context();

    Context(const Context& otherContext) = delete;
    Context& operator=(const Context& otherContext) = delete;
public:
    void InitVulkan(const Window* window);
private:
    VkInstance                  m_Instance;
    VkDebugUtilsMessengerEXT    m_DebugMessenger;
    VkPhysicalDevice            m_PhysicalDevice;
    VkDevice                    m_LogicalDevice;
    VkQueue	                    m_GraphicsQueue;
    VkQueue	                    m_PresentQueue;
    VkSurfaceKHR                m_Surface;
private:
    bool                        m_EnableValidation;
};
