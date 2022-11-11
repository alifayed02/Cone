#pragma once

#include "Core/CnPch.hpp"

#include "glm/glm.hpp"

struct Vertex
{
    glm::vec3 pos;
    glm::vec3 color;
    glm::vec2 texCoord;

    static constexpr VkVertexInputBindingDescription GetBindingDescription()
    {
        return VkVertexInputBindingDescription{
                .binding = 0U,
                .stride = sizeof(Vertex),
                .inputRate = VK_VERTEX_INPUT_RATE_VERTEX,
        };
    }

    static constexpr std::array<VkVertexInputAttributeDescription, 3> GetAttributeDescriptions()
    {
        return std::array<VkVertexInputAttributeDescription, 3> {
                VkVertexInputAttributeDescription
                {
                        .location   = 0U,
                        .binding    = 0U,
                        .format     = VK_FORMAT_R32G32B32_SFLOAT,
                        .offset     = offsetof(Vertex, pos),
                },
                VkVertexInputAttributeDescription
                {
                        .location   = 1U,
                        .binding    = 0U,
                        .format     = VK_FORMAT_R32G32B32_SFLOAT,
                        .offset     = offsetof(Vertex, color),
                },
                VkVertexInputAttributeDescription
                {
                         .location  = 2U,
                         .binding   = 0U,
                         .format    = VK_FORMAT_R32G32_SFLOAT,
                         .offset    = offsetof(Vertex, texCoord),
                }
        };
    }

    bool operator==(const Vertex& otherVertex) const
    {
        return pos == otherVertex.pos && color == otherVertex.color && texCoord == otherVertex.texCoord;
    }
};
