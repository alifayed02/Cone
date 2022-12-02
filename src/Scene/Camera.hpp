#pragma once

#include "Renderer/Swapchain.hpp"
#include "Renderer/DescriptorSet.hpp"
#include "Renderer/Buffer/Buffer.hpp"

#include "glm/glm.hpp"

class Context;
struct GLFWwindow;

class Camera
{
public:
    struct CameraBufferObject
    {
        glm::mat4 viewMatrix;
        glm::mat4 projectionMatrix;
        glm::mat4 viewProjectionMatrix;
    };
public:
    explicit Camera(Context* context);
    ~Camera() = default;
public:
    void WriteBuffer(uint32_t frameIndex);
    void SetExtent(VkExtent2D extent);
    void ProcessKeyboardInputs(GLFWwindow* window);
    void ProcessMouseMovements(GLFWwindow* window);
    void Update(uint32_t frameIndex);
public:
    inline VkDescriptorSetLayout GetCameraLayout() const { return m_DescriptorSets[0]->GetDescriptorSetLayout(); }
    inline VkDescriptorSet GetDescriptorSet(const uint32_t frameIndex) const { return m_DescriptorSets[frameIndex]->GetDescriptorSet(); }
private:
    void CreateDescriptorBuffers();
    void CreateDescriptorSet();
    void RotateVector(float angle, const glm::vec3& axis, glm::vec3& rotationVec);
    void UpdateCameraUVN();
    glm::mat4 CreateCameraMatrix();
    void PrintPosition();
private:
    Context*                                                                m_Context;
    VkExtent2D                                                              m_CameraExtent;
    std::array<std::unique_ptr<DescriptorSet>, Swapchain::FRAMES_IN_FLIGHT> m_DescriptorSets;
    std::array<std::unique_ptr<Buffer>, Swapchain::FRAMES_IN_FLIGHT>        m_Buffers;
    std::array<CameraBufferObject, Swapchain::FRAMES_IN_FLIGHT>             m_BufferObjects;
private:
    glm::vec3   m_Position;
    glm::vec3   m_Target;
    glm::vec3   m_Up;
    float       m_Speed;
    float       m_AngleHorizontal;
    float       m_AngleVertical;
    double      m_MousePosX;
    double      m_MousePosY;
};
