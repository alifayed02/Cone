#pragma once

#include "Asset/SubMesh.hpp"
#include "Renderer/Swapchain.hpp"
#include "Camera.hpp"
#include "Scene/Lighting/Lights.hpp"

#include "glm/glm.hpp"

class SceneMember;
class Context;
class Mesh;

class Scene
{
public:
    explicit Scene(Context* context);
    ~Scene() = default;

    Scene(const Scene& otherScene) = delete;
    Scene& operator=(const Scene& otherScene) = delete;
public:
    SceneMember* AddSceneMember(Mesh* mesh);
    SceneMember* GetSceneMember(std::string_view name);
    Lights::PointLight* AddPointLight(Lights::PointLight pointLight);
    Lights::DirectionalLight* AddDirectionalLight(Lights::DirectionalLight directionalLight);
public:
    inline Camera& GetCamera() { return m_Camera; }
    inline const std::vector<SceneMember>& GetSceneMembers() const { return m_SceneMembers; }
public:
    inline const std::vector<Lights::PointLight>& GetPointLights() const { return m_PointLights; }
    inline std::vector<Lights::PointLight>& GetPointLights() { return m_PointLights; }
public:
    inline const std::vector<Lights::DirectionalLight>& GetDirectionalLights() const { return m_DirectionalLights; }
    inline std::vector<Lights::DirectionalLight> GetDirectionalLights() { return m_DirectionalLights; }
private:
    Context*                                m_Context;
    Camera                                  m_Camera;
    std::vector<SceneMember>                m_SceneMembers;
    std::vector<Lights::PointLight>         m_PointLights;
    std::vector<Lights::DirectionalLight>   m_DirectionalLights;
};
