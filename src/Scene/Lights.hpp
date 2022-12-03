#pragma once

#include "glm/glm.hpp"

namespace Lights
{
    struct PointLight
    {
        glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f);
        alignas(16) glm::vec3 color = glm::vec3(1.0f, 1.0f, 1.0f);
        float radius{1.0f};
    };

    struct DirectionalLight
    {

    };

    struct SpotLight
    {

    };

    struct LightBufferObject
    {
        PointLight  pointlights[MAX_POINT_LIGHTS_SIZE];
        uint32_t    numPointLights{};
        alignas(16) glm::vec3 viewPos{0.0f};
        alignas(16) glm::mat4 viewMatrix{1.0f};
    };
}
