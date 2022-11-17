#include "Core/CnPch.hpp"
#include "Camera.hpp"

#include "Renderer/Context.hpp"

#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/quaternion.hpp"
#include "glm/gtx/quaternion.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "glfw/glfw3.h"

Camera::Camera(Context* context)
    :   m_Context{context}, m_CameraExtent{}, m_DescriptorPool{},
        m_DescriptorSetLayout{}, m_DescriptorSets{}, m_BufferObjects{},
        m_Position{0.0f}, m_Target{0.0f, 0.0f, -1.0f}, m_Up{0.0f, 1.0f, 0.0f},
        m_Speed{0.1f}, m_AngleHorizontal{-90.0f}, m_AngleVertical{0.0f},
        m_MousePosX{(double)m_CameraExtent.width/2}, m_MousePosY{(double)m_CameraExtent.height/2}
{
    CreateDescriptorPool();
    CreateDescriptorSet();
    CreateDescriptorBuffers();
}

void Camera::CreateDescriptorPool()
{
    // Pool will hold {2} Uniform Buffers
    VkDescriptorPoolSize poolSize{};
    poolSize.type               = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    poolSize.descriptorCount    = Swapchain::FRAMES_IN_FLIGHT;

    // Pool will hold 2 DescriptorSets
    VkDescriptorPoolCreateInfo poolCreateInfo{};
    poolCreateInfo.sType            = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    poolCreateInfo.maxSets          = Swapchain::FRAMES_IN_FLIGHT;
    poolCreateInfo.poolSizeCount    = 1;
    poolCreateInfo.pPoolSizes       = &poolSize;

    VK_CHECK(vkCreateDescriptorPool(m_Context->GetLogicalDevice(), &poolCreateInfo, nullptr, &m_DescriptorPool))
}

void Camera::CreateDescriptorSet()
{
    VkDescriptorSetLayoutBinding layoutBinding{};
    layoutBinding.binding           = 0;
    layoutBinding.descriptorType    = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    layoutBinding.descriptorCount   = 1;
    layoutBinding.stageFlags        = VK_SHADER_STAGE_VERTEX_BIT;

    VkDescriptorSetLayoutCreateInfo layoutInfo{};
    layoutInfo.sType        = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    layoutInfo.bindingCount = 1;
    layoutInfo.pBindings    = &layoutBinding;

    vkCreateDescriptorSetLayout(m_Context->GetLogicalDevice(), &layoutInfo, nullptr, &m_DescriptorSetLayout);
    std::vector<VkDescriptorSetLayout> setLayouts(Swapchain::FRAMES_IN_FLIGHT, m_DescriptorSetLayout);

    VkDescriptorSetAllocateInfo setAllocateInfo{};
    setAllocateInfo.sType               = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    setAllocateInfo.descriptorPool      = m_DescriptorPool;
    setAllocateInfo.descriptorSetCount  = setLayouts.size();
    setAllocateInfo.pSetLayouts         = setLayouts.data();

    VK_CHECK(vkAllocateDescriptorSets(m_Context->GetLogicalDevice(), &setAllocateInfo, m_DescriptorSets.data()))
}

void Camera::CreateDescriptorBuffers()
{
    for(size_t i = 0; i < m_Buffers.max_size(); i++)
    {
        Buffer::BufferInfo bufferInfo{};
        bufferInfo.size = sizeof(CameraBufferObject);
        bufferInfo.usageFlags = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
        bufferInfo.vmaMemoryUsage = VMA_MEMORY_USAGE_AUTO_PREFER_HOST;
        bufferInfo.vmaAllocFlags = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT | VMA_ALLOCATION_CREATE_MAPPED_BIT;

        m_Buffers[i] = std::make_unique<Buffer>(m_Context, bufferInfo);
    }

    for(size_t i = 0; i < m_DescriptorSets.size(); i++)
    {
        VkDescriptorBufferInfo bufferInfo{};
        bufferInfo.buffer   = m_Buffers[i]->GetBuffer();
        bufferInfo.offset   = 0;
        bufferInfo.range    = m_Buffers[i]->GetSize();


        VkWriteDescriptorSet writeSet{};
        writeSet.sType              = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        writeSet.dstSet             = m_DescriptorSets[i];
        writeSet.dstBinding         = 0;
        writeSet.dstArrayElement    = 0;
        writeSet.descriptorCount    = 1;
        writeSet.descriptorType     = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        writeSet.pBufferInfo        = &bufferInfo;

        vkUpdateDescriptorSets(m_Context->GetLogicalDevice(), 1, &writeSet, 0, nullptr);
    }
}

void Camera::WriteBuffer(const uint32_t frameIndex)
{
    m_Buffers[frameIndex]->Map(&m_BufferObjects[frameIndex], m_Buffers[frameIndex]->GetSize());
}

void Camera::Bind(VkCommandBuffer commandBuffer, VkPipelineLayout pipelineLayout, const uint32_t frameIndex, const uint32_t setIndex) const
{
    vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, setIndex, 1U, &m_DescriptorSets[frameIndex], 0U, nullptr);
}

void Camera::SetExtent(const VkExtent2D extent)
{
    m_CameraExtent = extent;

    for(size_t i = 0; i < m_BufferObjects.size(); i++)
    {
        m_BufferObjects[i].projectionMatrix = glm::perspective(glm::radians(45.0f), (float)m_CameraExtent.width/(float)m_CameraExtent.height, 0.1f, 100.0f);
        m_BufferObjects[i].projectionMatrix[1][1] *= -1;
        m_BufferObjects[i].viewMatrix = glm::mat4(1.0f);

        m_BufferObjects[i].viewProjectionMatrix = m_BufferObjects[i].projectionMatrix * m_BufferObjects[i].viewMatrix;

        WriteBuffer(i);
    }
}

void Camera::ProcessKeyboardInputs(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, true);
    }
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
    {
        m_Position -= m_Speed * glm::normalize(glm::vec3(m_Target.x, 0.0f, m_Target.z));
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
    {
        m_Position += m_Speed * glm::normalize(glm::vec3(m_Target.x, 0.0f, m_Target.z));
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
    {
        glm::vec3 leftVec = glm::normalize(glm::cross(m_Target, m_Up));
        m_Position += m_Speed * leftVec;
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
    {
        glm::vec3 rightVec = glm::normalize(glm::cross(m_Up, m_Target));
        m_Position += m_Speed * rightVec;
    }
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
    {
        m_Position += m_Speed * glm::vec3(0.0f, 1.0f, 0.0f);
    }
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
    {
        m_Position -= m_Speed * glm::vec3(0.0f, 1.0f, 0.0f);
    }
}

void Camera::ProcessMouseMovements(GLFWwindow* window)
{
    double xPos;
    double yPos;
    glfwGetCursorPos(window, &xPos, &yPos);

    double deltaXPos = xPos - m_MousePosX;
    double deltaYPos = yPos - m_MousePosY;

    if (deltaXPos == 0 && deltaYPos == 0)
    {
        return;
    }

    m_MousePosX = xPos;
    m_MousePosY = yPos;

    m_AngleHorizontal   -= (float)deltaXPos / 10.0f;
    m_AngleVertical     -= (float)deltaYPos / 10.0f;

    if (m_AngleVertical >= 89.0f)
    {
        m_AngleVertical = 89.0f;
    }
    if (m_AngleVertical <= -89.0f)
    {
        m_AngleVertical = -89.0f;
    }

    UpdateCameraUVN();
}

glm::mat4 Camera::CreateCameraMatrix()
{
    glm::vec3 nAxis = m_Target;
    glm::normalize(nAxis);

    glm::vec3 uAxis = glm::cross(m_Up, nAxis);
    glm::normalize(uAxis);

    glm::vec3 vAxis = glm::cross(nAxis, uAxis);

    float viewArray[] =
            {
                    uAxis.x, uAxis.y, uAxis.z, 0.0f,
                    vAxis.x, vAxis.y, vAxis.z, 0.0f,
                    nAxis.x, nAxis.y, nAxis.z, 0.0f,
                    0.0f,    0.0f,	  0.0f,    1.0f
            };

    glm::mat4 view = glm::transpose(glm::make_mat4(viewArray));
    view = glm::translate(view, -m_Position);

    return view;
}

void Camera::RotateVector(float angle, const glm::vec3& axis, glm::vec3& rotationVec)
{
    glm::quat q = glm::angleAxis(glm::radians(angle), axis);;
    glm::quat qPrime = glm::conjugate(q);
    glm::quat w = q * glm::quat(0.0f, rotationVec.x, rotationVec.y, rotationVec.z) * qPrime;

    rotationVec.x = w.x;
    rotationVec.y = w.y;
    rotationVec.z = w.z;
}

void Camera::UpdateCameraUVN()
{
    glm::vec3 yAxis(0.0f, 1.0f, 0.0f);

    glm::vec3 view(1.0f, 0.0f, 0.0f);
    RotateVector(m_AngleHorizontal, yAxis, view);
    glm::normalize(view);

    glm::vec3 u = glm::cross(yAxis, view);
    glm::normalize(u);
    RotateVector(m_AngleVertical, u, view);

    m_Target = view;
    glm::normalize(m_Target);

    m_Up = glm::cross(m_Target, u);
    glm::normalize(m_Up);
}

void Camera::Update(const uint32_t frameIndex)
{
    m_BufferObjects[frameIndex].viewMatrix = CreateCameraMatrix();
    m_BufferObjects[frameIndex].viewProjectionMatrix = m_BufferObjects[frameIndex].projectionMatrix * m_BufferObjects[frameIndex].viewMatrix;
    WriteBuffer(frameIndex);
}

Camera::~Camera()
{
    if(m_DescriptorSetLayout)
    {
        vkDestroyDescriptorSetLayout(m_Context->GetLogicalDevice(), m_DescriptorSetLayout, nullptr);
    }
    if(m_DescriptorPool)
    {
        vkDestroyDescriptorPool(m_Context->GetLogicalDevice(), m_DescriptorPool, nullptr);
    }
}