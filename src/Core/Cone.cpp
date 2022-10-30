#include "CnPch.hpp"
#include "Cone.hpp"

#include "Scene/Mesh.hpp"

void Cone::Init()
{
    VkExtent2D extent = {1280, 720};
    m_Window    = std::make_unique<Window>(extent, "Cone Engine");
    m_Context   = std::make_unique<Context>(m_Window.get());

    CreateMainScene();
    m_Renderer = std::make_unique<Renderer>(m_Context.get(), m_MainScene.get());
    m_Renderer->SetActiveScene(m_MainScene.get());

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

    vkDeviceWaitIdle(m_Context->GetLogicalDevice());
    std::cout << "[Cone] Successfully Closed\n";
}

void Cone::Draw()
{
    m_Renderer->DrawFrame();
}

void Cone::CreateMainScene()
{
    m_MainScene = std::make_unique<Scene>(m_Context.get());

    std::vector<Vertex> vertices
            {
                    {{-0.5f,  0.5f, -2.5f},  {1.0f, 1.0f, 1.0f}, {1.0f, 1.0f}},
                    {{-0.5f, -0.5f, -2.5f},  {1.0f, 1.0f, 1.0f}, {1.0f, 0.0f}},
                    {{ 0.5f, -0.5f, -2.5f},  {1.0f, 1.0f, 1.0f}, {0.0f, 0.0f}},
                    {{ 0.5f,  0.5f, -2.5f},  {1.0f, 1.0f, 1.0f}, {0.0f, 1.0f}}
            };

    std::vector<Vertex> vertices2
            {
                    {{-0.5f,  0.5f, -4.5f},  {1.0f, 0.0f, 1.0f}, {1.0f, 1.0f}},
                    {{-0.5f, -0.5f, -4.5f},  {1.0f, 0.0f, 1.0f}, {1.0f, 0.0f}},
                    {{ 0.5f, -0.5f, -4.5f},  {1.0f, 0.0f, 1.0f}, {0.0f, 0.0f}},
                    {{ 0.5f,  0.5f, -4.5f},  {1.0f, 0.0f, 1.0f}, {0.0f, 1.0f}}
            };

    std::vector<uint16_t> indices
            {
                    0, 1, 2, 2, 3, 0
            };

    Mesh::MeshInfo rectangle{};
    rectangle.vertices  = vertices;
    rectangle.indices   = indices;
    rectangle.texturePath = R"(D:\Graphics\Cone\Textures\stanfordbunny.jpg)";

    Mesh::MeshInfo rectangle2{};
    rectangle2.vertices = vertices2;
    rectangle2.indices = indices;
    rectangle2.texturePath = R"(D:\Graphics\Cone\Textures\blendermonkey.jpg)";

    m_MainScene->AddSceneMember(rectangle);
    m_MainScene->AddSceneMember(rectangle2);
}
