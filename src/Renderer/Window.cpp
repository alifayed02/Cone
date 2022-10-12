#include "Window.hpp"

Window::Window(const VkExtent2D extent, const std::string& windowTitle)
    :   m_Window{}, m_Extent(extent), m_WindowTitle(windowTitle)
{
    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

    m_Window = glfwCreateWindow(static_cast<int>(m_Extent.width), static_cast<int>(m_Extent.height), windowTitle.c_str(), nullptr, nullptr);
}

Window::~Window()
{
    glfwDestroyWindow(m_Window);
    glfwTerminate();
}


