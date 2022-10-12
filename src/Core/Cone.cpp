#include "Cone.hpp"

void Cone::Init()
{
    VkExtent2D extent = {1280, 720};

    m_Window = std::make_unique<Window>(extent, "Cone Engine");
    m_Context = std::make_unique<Context>(m_Window.get());

    std::cout << "[Cone] Successfully Initialized\n";
}

void Cone::Run()
{
    Init();

    while(!m_Window->ShouldClose())
    {
        m_Window->PollEvents();

        Draw();
    }

    std::cout << "[Cone] Successfully Closed\n";
}

void Cone::Draw()
{
}