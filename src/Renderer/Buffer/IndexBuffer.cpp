#include "Core/CnPch.hpp"
#include "IndexBuffer.hpp"

IndexBuffer::IndexBuffer(Context* context, const std::vector<uint16_t>& indices)
    :   m_Context{context}, m_Buffer{context, {indices.size() * sizeof(uint16_t), VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, 0}},
        m_IndicesCount{static_cast<uint32_t>(indices.size())}
{
    Buffer::BufferInfo stagingBufferInfo{};
    stagingBufferInfo.size = indices.size() * sizeof(uint16_t);
    stagingBufferInfo.usageFlags = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
    stagingBufferInfo.vmaAllocFlags = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT | VMA_ALLOCATION_CREATE_MAPPED_BIT;

    Buffer stagingBuffer{context, stagingBufferInfo};
    stagingBuffer.Map(indices.data(), stagingBufferInfo.size);
    stagingBuffer.Transfer(&m_Buffer);
}

void IndexBuffer::Bind(VkCommandBuffer commandBuffer) const
{
    vkCmdBindIndexBuffer(commandBuffer, m_Buffer.GetBuffer(), 0, VK_INDEX_TYPE_UINT16);
}
