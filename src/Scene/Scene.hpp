#pragma once

#include "Camera.hpp"
#include "Asset/SubMesh.hpp"

#include "Renderer/Swapchain.hpp"

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
    void AddSceneMember(Mesh* mesh);
public:
    inline const std::vector<std::unique_ptr<SceneMember>>& GetSceneMembers() const { return m_SceneMembers; }
    inline Camera& GetCamera() { return m_Camera; }
private:
    Context*                                                    m_Context;
    Camera                                                      m_Camera;
    std::vector<std::unique_ptr<SceneMember>>                   m_SceneMembers;
};
