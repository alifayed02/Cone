#pragma once

#include "Renderer/Window.hpp"
#include "Renderer/Context.hpp"
#include "Renderer/Renderer.hpp"
#include "Scene/Scene.hpp"
#include "Scene/SceneMember.hpp"
#include "Asset/AssetManager.hpp"

class Cone
{
public:
    Cone() = default;
    ~Cone() = default;
public:
    void Run();
private:
    void Init();
    void Draw();
    void CreateMainScene();
private:
    std::unique_ptr<Window>         m_Window;
    std::unique_ptr<Context>        m_Context;
    std::unique_ptr<AssetManager>   m_AssetManager;
    std::unique_ptr<Renderer>       m_Renderer;
    std::unique_ptr<Scene>          m_MainScene;
};
