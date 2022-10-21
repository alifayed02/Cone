#include "Core/CnPch.hpp"
#include "Context.hpp"

#include "Window.hpp"

Context::Context(const Window* window)
        :   m_Instance{}, m_DebugMessenger{}, m_PhysicalDevice{},
            m_LogicalDevice{}, m_GraphicsQueue{},
            m_PresentQueue{}, m_GraphicsQueueFamily{},
            m_CommandPool{}, m_Surface{}, m_SurfaceExtent{window->GetExtent2D()},
            m_EnableValidation(true)
{
#ifdef NDEBUG
    m_EnableValidation = false;
#endif
    InitVulkan(window);
    InitCommandPool();
}

VkCommandBuffer Context::BeginSingleTimeCommands()
{
    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandPool = m_CommandPool;
    allocInfo.commandBufferCount = 1U;

    VkCommandBuffer commandBuffer;
    vkAllocateCommandBuffers(m_LogicalDevice, &allocInfo, &commandBuffer);

    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

    vkBeginCommandBuffer(commandBuffer, &beginInfo);

    return commandBuffer;
}

void Context::EndSingleTimeCommands(VkCommandBuffer commandBuffer)
{
    vkEndCommandBuffer(commandBuffer);

    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1U;
    submitInfo.pCommandBuffers = &commandBuffer;

    vkQueueSubmit(m_GraphicsQueue, 1U, &submitInfo, VK_NULL_HANDLE);
    vkQueueWaitIdle(m_GraphicsQueue);

    vkFreeCommandBuffers(m_LogicalDevice, m_CommandPool, 1U, &commandBuffer);
}

void Context::InitVulkan(const Window* window)
{
    /*
     * Instance
     */
    vkb::InstanceBuilder instanceBuilder;
    auto inst_ret = instanceBuilder.set_app_name("Cone Engine")
            .request_validation_layers(m_EnableValidation)
            .require_api_version(1, 3, 0)
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
    m_GraphicsQueueFamily = vkbLogicalDevice.get_queue_index(vkb::QueueType::graphics).value();
}

void Context::InitCommandPool()
{
    VkCommandPoolCreateInfo commandPoolCreateInfo{};
    commandPoolCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    commandPoolCreateInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    commandPoolCreateInfo.queueFamilyIndex = m_GraphicsQueueFamily;
    VK_CHECK(vkCreateCommandPool(m_LogicalDevice, &commandPoolCreateInfo, nullptr, &m_CommandPool))
}

Context::~Context()
{
    vkDeviceWaitIdle(m_LogicalDevice);

    if(m_CommandPool)
    {
        vkDestroyCommandPool(m_LogicalDevice, m_CommandPool, nullptr);
    }

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