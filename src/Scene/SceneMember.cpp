#include "Core/CnPch.hpp"
#include "SceneMember.hpp"

#include "glm/gtc/matrix_transform.hpp"

#include "Asset/Mesh.hpp"

SceneMember::SceneMember(Mesh* mesh)
    :   m_Mesh{mesh}, m_Translation{0.0f},
        m_Rotation{0.0f}, m_Scale{1.0f}, m_ModelMatrix{1.0f}
{
}

SceneMember& SceneMember::Translate(float x, float y, float z)
{
    m_Translation.x = x;
    m_Translation.y = y;
    m_Translation.z = z;

    return *this;
}

SceneMember& SceneMember::Rotate(float angleX, float angleY, float angleZ)
{
    m_Rotation.x = angleX;
    m_Rotation.y = angleY;
    m_Rotation.z = angleZ;

    return *this;
}

SceneMember& SceneMember::Scale(float x, float y, float z)
{
    m_Scale.x = x;
    m_Scale.y = y;
    m_Scale.z = z;

    return *this;
}

void SceneMember::UpdateModelMatrix()
{
    // T R S
    m_ModelMatrix = glm::mat4(1.0f);
    m_ModelMatrix = glm::translate(m_ModelMatrix, m_Translation);

    m_ModelMatrix = glm::rotate(m_ModelMatrix, m_Rotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
    m_ModelMatrix = glm::rotate(m_ModelMatrix, m_Rotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
    m_ModelMatrix = glm::rotate(m_ModelMatrix, m_Rotation.z, glm::vec3(0.0f, 0.0f, 1.0f));

    m_ModelMatrix = glm::scale(m_ModelMatrix, m_Scale);
}