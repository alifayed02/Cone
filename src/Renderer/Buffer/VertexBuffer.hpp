#pragma once

#include "Buffer.hpp"
#include "Vertex.hpp"

class Context;

class VertexBuffer
{
public:
    VertexBuffer(Context* context, const std::vector<Vertex>& vertices);
    ~VertexBuffer() = default;

    VertexBuffer(VertexBuffer&& otherBuffer) = default;

    VertexBuffer(const VertexBuffer& otherVertexBuffer) = delete;
    VertexBuffer& operator=(const VertexBuffer& otherVertexBuffer) = delete;
public:
    inline uint32_t GetVerticesCount() const { return m_VerticesCount; }
public:
    void Bind(VkCommandBuffer cmdBuffer) const;
private:
    Buffer      m_Buffer;
    uint32_t    m_VerticesCount;
};
