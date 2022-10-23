#include "CnPch.hpp"
#include "Cone.hpp"

void Cone::Init()
{
    VkExtent2D extent = {1280, 720};

    m_Window    = std::make_unique<Window>(extent, "Cone Engine");
    m_Context   = std::make_unique<Context>(m_Window.get());
    m_Renderer  = std::make_unique<Renderer>(m_Context.get());

    std::cout << "[Cone] Successfully Initialized\n";
}

void Cone::Run()
{
    Init();

    std::vector<Vertex> vertices
            {
                    {{-0.5f,  0.5f, 0.0f}, {1.0f, 0.0f, 0.0f}},
                    {{-0.5f, -0.5f, 0.0f}, {1.0f, 0.0f, 0.0f}},
                    {{ 0.5f, -0.5f, 0.0f}, {0.0f, 0.0f, 1.0f}},
                    {{ 0.5f,  0.5f, 0.0f}, {0.0f, 1.0f, 0.0f}}
            };

    std::vector<uint16_t> indices
            {
                    0, 1, 2, 2, 3, 0
            };

    VertexBuffer vertexBuffer{m_Context.get(), vertices};
    IndexBuffer indexBuffer{m_Context.get(), indices};

    while(!m_Window->ShouldClose())
    {
        m_Window->PollEvents();

        Draw(vertexBuffer, indexBuffer);
    }

    vkDeviceWaitIdle(m_Context->GetLogicalDevice());
    std::cout << "[Cone] Successfully Closed\n";
}

void Cone::Draw(const VertexBuffer& vertexBuffer, const IndexBuffer& indexBuffer)
{
    m_Renderer->DrawFrame(vertexBuffer, indexBuffer);
}