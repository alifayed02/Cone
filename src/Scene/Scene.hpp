#pragma once

#include "Asset/SubMesh.hpp"
#include "Renderer/Swapchain.hpp"
#include "Camera.hpp"

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
public:
    inline const std::vector<SceneMember>& GetSceneMembers() const { return m_SceneMembers; }
    inline Camera& GetCamera() { return m_Camera; }
private:
    Context*                    m_Context;
    Camera                      m_Camera;
    std::vector<SceneMember>    m_SceneMembers;
    // std::vector<DirectionalLight>  m_DirectionalLights;
};
