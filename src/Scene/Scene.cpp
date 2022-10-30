#include "Core/CnPch.hpp"
#include "Scene.hpp"

#include "SceneMember.hpp"

#include "Renderer/Context.hpp"

Scene::Scene(Context* context)
    :   m_Context{context}, m_Camera{context}
{
}

void Scene::AddSceneMember(const Mesh::MeshInfo& meshInfo)
{
    m_SceneMembers.emplace_back(std::make_unique<SceneMember>(m_Context, meshInfo));
}
