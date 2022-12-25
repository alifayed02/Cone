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
    if(m_PointLights.size() >= MAX_POINT_LIGHTS_SIZE)
    {
        throw std::runtime_error("Error: Cannot add more point lights.");
    }
    m_PointLights.emplace_back(pointLight);
    return &m_PointLights.back();
}

Lights::DirectionalLight* Scene::AddDirectionalLight(Lights::DirectionalLight directionalLight)
{
    if(m_DirectionalLights.size() >= MAX_DIRECTIONAL_LIGHTS_SIZE)
    {
        throw std::runtime_error("Error: Cannot add more directional lights.");
    }
    m_DirectionalLights.emplace_back(directionalLight);
    return &m_DirectionalLights.back();
}
