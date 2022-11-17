#include "CnPch.hpp"
#include "Cone.hpp"

#include "Asset/SubMesh.hpp"

void Cone::Init()
{
    VkExtent2D extent = {1280, 720};
    m_Window        = std::make_unique<Window>(extent, "Cone Engine");
    m_Context       = std::make_unique<Context>(m_Window.get());
    m_AssetManager  = std::make_unique<AssetManager>(m_Context.get());

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

    m_AssetManager->LoadMesh("Lantern", "/Assets/Models/Lantern/Lantern.gltf");
    m_MainScene->AddSceneMember(m_AssetManager->GetMesh("Lantern"));
}
