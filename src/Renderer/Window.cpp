#include <Core/CnPch.hpp>
#include "Window.hpp"

Window::Window(const VkExtent2D extent, std::string_view windowTitle)
    :   m_Window{}, m_Extent(extent), m_WindowTitle(windowTitle)
{
    glfwInit();

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    m_Window = glfwCreateWindow(static_cast<int>(m_Extent.width), static_cast<int>(m_Extent.height), std::string(windowTitle).c_str(), nullptr, nullptr);
}

Window::~Window()
{
    glfwDestroyWindow(m_Window);
    glfwTerminate();
}


