#pragma once

#include "Buffer.hpp"

class Context;

class IndexBuffer
{
public:
    IndexBuffer(Context* context, const std::vector<uint16_t>& indices);
    ~IndexBuffer() = default;

    IndexBuffer(const IndexBuffer& otherIndexBuffer) = delete;
    IndexBuffer& operator=(const IndexBuffer& otherIndexBuffer) = delete;
public:
    inline uint32_t GetIndicesCount() const { return m_IndicesCount; }
public:
    void Bind(VkCommandBuffer commandBuffer) const;
private:
    Context*    m_Context;
    Buffer      m_Buffer;
    uint32_t    m_IndicesCount;
};
