#pragma once

#include "Asset/SubMesh.hpp"
#include "Renderer/Swapchain.hpp"
#include "Camera.hpp"
#include "Lights.hpp"

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
public:
    inline Camera& GetCamera() { return m_Camera; }
    inline const std::vector<SceneMember>& GetSceneMembers() const { return m_SceneMembers; }
    inline const std::vector<Lights::PointLight>& GetPointLights() const { return m_PointLights; }
private:
    Context*                        m_Context;
    Camera                          m_Camera;
    std::vector<SceneMember>        m_SceneMembers;
    std::vector<Lights::PointLight> m_PointLights;
};
