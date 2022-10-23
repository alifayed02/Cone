#pragma once

#include "Renderer/Window.hpp"
#include "Renderer/Context.hpp"
#include "Renderer/Renderer.hpp"

#include "Renderer/Buffer/VertexBuffer.hpp"
#include "Renderer/Buffer/IndexBuffer.hpp"

class Cone
{
public:
    Cone() = default;
    ~Cone() = default;
public:
    void Run();
private:
    void Init();
    void Draw(const VertexBuffer& vertexBuffer, const IndexBuffer& indexBuffer);
private:
    std::unique_ptr<Window>     m_Window;
    std::unique_ptr<Context>    m_Context;
    std::unique_ptr<Renderer>   m_Renderer;
};
