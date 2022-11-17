#include "Core/CnPch.hpp"
#include "Scene.hpp"

#include "Renderer/Context.hpp"
#include "Asset/Mesh.hpp"
#include "SceneMember.hpp"

Scene::Scene(Context* context)
    :   m_Context{context}, m_Camera{context}
{
}

SceneMember* Scene::AddSceneMember(Mesh* mesh)
{
    m_SceneMembers.emplace_back(std::make_unique<SceneMember>(mesh));
    return m_SceneMembers.back().get();
}
