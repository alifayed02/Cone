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
    m_SceneMembers.emplace_back(mesh);
    return &m_SceneMembers.back();
}

SceneMember* Scene::GetSceneMember(std::string_view name)
{
    for(auto& sceneMember : m_SceneMembers)
    {
        if(std::strcmp(sceneMember.GetMesh()->GetName().data(), name.data()) == 0)
        {
            return &sceneMember;
        }
    }
    return nullptr;
}

Lights::PointLight* Scene::AddPointLight(const Lights::PointLight pointLight)
{
    m_PointLights.emplace_back(pointLight);
    return &m_PointLights.back();
}
