#include "CnPch.hpp"
#include "Cone.hpp"

#include "Asset/SubMesh.hpp"

void Cone::Init()
{
    VkExtent2D extent = {1920, 1080};
    m_Window        = std::make_unique<Window>(extent, "Cone Engine");
    m_Context       = std::make_unique<Context>(m_Window.get());
    m_AssetManager  = std::make_unique<AssetManager>(m_Context.get());

    CreateMainScene();
    m_Renderer = std::make_unique<Renderer>(m_Context.get(), m_MainScene.get());
    m_Renderer->SetActiveScene(m_MainScene.get());

    glfwSetInputMode(m_Window->GetGLFWWindow(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPos(m_Window->GetGLFWWindow(), m_Window->GetWidth()/2, m_Window->GetHeight()/2);

    std::cout << "[Cone] Successfully Initialized\n";
}

void Cone::Run()
{
    Init();

    while(!m_Window->ShouldClose())
    {
        m_Window->PollEvents();

        m_MainScene->GetCamera().ProcessKeyboardInputs(m_Window->GetGLFWWindow());
        m_MainScene->GetCamera().ProcessMouseMovements(m_Window->GetGLFWWindow());
        m_MainScene->GetCamera().Update(m_Renderer->GetCurrentFrame());

        UpdateMainScene();

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

    // Models
    m_AssetManager->LoadMesh("Sponza", "/Assets/Models/Sponza/Sponza.gltf");
    SceneMember* sponza = m_MainScene->AddSceneMember(m_AssetManager->GetMesh("Sponza"));
    sponza->Scale(0.01f, 0.01f, 0.01f).Translate(0.0f, -0.3f, -1.0f).UpdateModelMatrix();

    // Camera Settings
    m_MainScene->GetCamera().SetExposure(1.0f);

    // Point Lights
//    m_MainScene->AddPointLight({ glm::vec3(8.21f, 1.65f, -1.36f), glm::vec3(0.6f, 0.6f, 0.6f), 10.0f });

    // Directional Lights
    m_MainScene->AddDirectionalLight({ glm::vec3(-2.41f, -0.65f, -0.23f), glm::vec3(0.6f, 0.6f, 0.6f) });
}

void Cone::UpdateMainScene()
{
}
