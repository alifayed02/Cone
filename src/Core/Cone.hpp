#pragma once

#include "Renderer/Window.hpp"
#include "Renderer/Context.hpp"
#include "Renderer/Renderer.hpp"

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
private:
    std::string test;
    std::unique_ptr<Window>     m_Window;
    std::unique_ptr<Context>    m_Context;
    std::unique_ptr<Renderer>   m_Renderer;
};
