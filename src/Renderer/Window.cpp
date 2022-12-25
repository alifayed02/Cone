#include <Core/CnPch.hpp>
#include "Window.hpp"

#include "stb/stb_image.h"

Window::Window(const VkExtent2D extent, std::string_view windowTitle)
    :   m_Window{}, m_Extent(extent), m_WindowTitle(windowTitle)
{
    glfwInit();

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    m_Window = glfwCreateWindow(static_cast<int>(m_Extent.width), static_cast<int>(m_Extent.height), std::string(windowTitle).c_str(), nullptr, nullptr);

    InitIcon();
}

void Window::InitIcon()
{
    GLFWimage images[1];
    images[0].pixels = stbi_load(R"(D:\Graphics\Cone\Assets\Icons\cone.png)", &images[0].width, &images[0].height, nullptr, 4);
    glfwSetWindowIcon(m_Window, 1, images);
    stbi_image_free(images[0].pixels);
}

Window::~Window()
{
    glfwDestroyWindow(m_Window);
    glfwTerminate();
}