#pragma once

#include "Core/CnPch.hpp"

#include "glm/glm.hpp"

struct Vertex
{
    glm::vec3 pos;
    glm::vec3 normal;
    glm::vec2 texcoord;

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
                        .offset = offsetof(Vertex, normal),
                },
                VkVertexInputAttributeDescription
                {
                        .location = 3U,
                        .binding = 0U,
                        .format = VK_FORMAT_R32G32_SFLOAT,
                        .offset = offsetof(Vertex, texcoord),
                }
        };
    }

    bool operator==(const Vertex& other) const
    {
        return pos == other.pos && texcoord == other.texcoord && normal == other.normal;
    }
};
