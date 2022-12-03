#pragma once

#include "glm/glm.hpp"

struct Vertex
{
    glm::vec3 pos;
    glm::vec3 normal;
    glm::vec4 tangent;
    glm::vec2 texCoord;

    static constexpr VkVertexInputBindingDescription GetBindingDescription()
    {
        return VkVertexInputBindingDescription{
                .binding = 0U,
                .stride = sizeof(Vertex),
                .inputRate = VK_VERTEX_INPUT_RATE_VERTEX,
        };
    }

    static constexpr std::array<VkVertexInputAttributeDescription, 4> GetAttributeDescriptions()
    {
        return std::array<VkVertexInputAttributeDescription, 4> {
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
                        .offset     = offsetof(Vertex, normal),
                },
                VkVertexInputAttributeDescription
                {
                        .location   = 2U,
                        .binding    = 0U,
                        .format     = VK_FORMAT_R32G32B32A32_SFLOAT,
                        .offset     = offsetof(Vertex, tangent),
                },
                VkVertexInputAttributeDescription
                {
                         .location  = 3U,
                         .binding   = 0U,
                         .format    = VK_FORMAT_R32G32_SFLOAT,
                         .offset    = offsetof(Vertex, texCoord),
                }
        };
    }

    bool operator==(const Vertex& otherVertex) const
    {
        return pos == otherVertex.pos && normal == otherVertex.normal && tangent == otherVertex.tangent && texCoord == otherVertex.texCoord;
    }
};
