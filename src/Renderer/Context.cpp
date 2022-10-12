#include "Context.hpp"

#include "Window.hpp"

#include "vk-bootstrap/src/VkBootstrap.h"

Context::Context(const Window* window)
    :   m_Instance{}, m_DebugMessenger{}, m_PhysicalDevice{},
        m_LogicalDevice{}, m_GraphicsQueue{},
        m_PresentQueue{}, m_Surface{}, m_EnableValidation(true)
{
#ifdef NDEBUG
    m_EnableValidation = false;
#endif
    InitVulkan(window);
}

void Context::InitVulkan(const Window* window)
{
    /*
     * Instance
     */
    vkb::InstanceBuilder instanceBuilder;
    auto inst_ret = instanceBuilder.set_app_name("Cone Engine")
                    .request_validation_layers(m_EnableValidation)
                    .require_api_version(1, 1, 0)
                    .enable_extension("VK_KHR_surface")
                    .use_default_debug_messenger();

#ifdef __APPLE__
    instanceBuilder.enable_extension("VK_KHR_get_physical_device_properties2");
#endif

    vkb::Instance vkbInstance = inst_ret.build().value();
    m_Instance = vkbInstance.instance;
    m_DebugMessenger = vkbInstance.debug_messenger;

    /*
     * Surface
     */
    VK_CHECK(glfwCreateWindowSurface(m_Instance, window->GetGLFWWindow(), nullptr, &m_Surface))

    /*
     * Device
     */
    vkb::PhysicalDeviceSelector pDeviceSelector{vkbInstance};
    vkb::PhysicalDevice vkbPhysicalDevice = pDeviceSelector.set_minimum_version(1, 1)
                                                            .set_surface(m_Surface)
                                                            .select()
                                                            .value();
    vkb::DeviceBuilder deviceBuilder{vkbPhysicalDevice};
    vkb::Device vkbLogicalDevice = deviceBuilder.build().value();
    m_PhysicalDevice = vkbPhysicalDevice.physical_device;
    m_LogicalDevice = vkbLogicalDevice.device;

    m_GraphicsQueue = vkbLogicalDevice.get_queue(vkb::QueueType::graphics).value();
    m_PresentQueue = vkbLogicalDevice.get_queue(vkb::QueueType::present).value();
}

Context::~Context()
{
    vkDeviceWaitIdle(m_LogicalDevice);

    if(m_LogicalDevice)
    {
        vkDestroyDevice(m_LogicalDevice, nullptr);
    }

    if(m_Surface)
    {
        vkDestroySurfaceKHR(m_Instance, m_Surface, nullptr);
    }

    if(m_DebugMessenger)
    {
        vkb::destroy_debug_utils_messenger(m_Instance, m_DebugMessenger);
    }

    if(m_Instance)
    {
        vkDestroyInstance(m_Instance, nullptr);
    }
}
