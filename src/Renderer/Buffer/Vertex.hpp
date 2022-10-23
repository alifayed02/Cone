#pragma once

#include "Core/CnPch.hpp"

#include "glm/glm.hpp"

struct Vertex
{
    glm::vec3 pos;
    glm::vec3 color;

    static constexpr VkVertexInputBindingDescription GetBindingDescription()
    {
        return VkVertexInputBindingDescription{
                .binding = 0U,
                .stride = sizeof(Vertex),
                .inputRate = VK_VERTEX_INPUT_RATE_VERTEX,
        };
    }

    static constexpr std::array<VkVertexInputAttributeDescription, 2> GetAttributeDescriptions()
    {
        return std::array<VkVertexInputAttributeDescription, 2> {
                VkVertexInputAttributeDescription
                {
                        .location = 0U,
                        .binding = 0U,
                        .format = VK_FORMAT_R32G32B32_SFLOAT,
                        .offset = offsetof(Vertex, pos),
                },
                VkVertexInputAttributeDescription
                {
                        .location = 1U,
                        .binding = 0U,
                        .format = VK_FORMAT_R32G32B32_SFLOAT,
                        .offset = offsetof(Vertex, color),
                },
        };
    }

    bool operator==(const Vertex& otherVertex) const
    {
        return pos == otherVertex.pos && color == otherVertex.color;
    }
};
