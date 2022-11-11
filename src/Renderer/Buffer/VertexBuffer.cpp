#include "Core/CnPch.hpp"
#include "VertexBuffer.hpp"

VertexBuffer::VertexBuffer(Context* context, const std::vector<Vertex>& vertices)
    :   m_Context{context}, m_Buffer{context, {vertices.size() * sizeof(Vertex), VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VMA_MEMORY_USAGE_AUTO, 0}},
        m_VerticesCount{static_cast<uint32_t>(vertices.size())}
{
    Buffer::BufferInfo stagingBufferInfo{};
    stagingBufferInfo.size              = vertices.size() * sizeof(Vertex);
    stagingBufferInfo.usageFlags        = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
    stagingBufferInfo.vmaMemoryUsage    = VMA_MEMORY_USAGE_AUTO_PREFER_HOST;
    stagingBufferInfo.vmaAllocFlags     = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT | VMA_ALLOCATION_CREATE_MAPPED_BIT;

    Buffer stagingBuffer{context, stagingBufferInfo};
    stagingBuffer.Map(vertices.data(), stagingBufferInfo.size);
    stagingBuffer.Transfer(&m_Buffer);
}

void VertexBuffer::Bind(VkCommandBuffer cmdBuffer) const
{
    VkDeviceSize offsets[] = { 0 };
    vkCmdBindVertexBuffers(cmdBuffer, 0, 1, &m_Buffer.GetBuffer(), offsets);
}