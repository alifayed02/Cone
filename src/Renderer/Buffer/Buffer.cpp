#include "Core/CnPch.hpp"
#include "Buffer.hpp"

#include "Renderer/Context.hpp"

Buffer::Buffer(Context* context, const BufferInfo& bufferInfo)
    :   m_Context{context}, m_Buffer{}, m_Allocation{}, m_AllocInfo{}
{
    VkBufferCreateInfo bufferCreateInfo{};
    bufferCreateInfo.sType          = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferCreateInfo.size           = bufferInfo.size;
    bufferCreateInfo.usage          = bufferInfo.usageFlags;
    bufferCreateInfo.sharingMode    = VK_SHARING_MODE_EXCLUSIVE;

    VmaAllocationCreateInfo allocInfo{};
    allocInfo.usage = VMA_MEMORY_USAGE_AUTO;
    allocInfo.flags = bufferInfo.vmaAllocFlags;

    vmaCreateBuffer(m_Context->GetAllocator(), &bufferCreateInfo, &allocInfo, &m_Buffer, &m_Allocation, &m_AllocInfo);
}

void Buffer::Map(const void* memory, VkDeviceSize size) const
{
    memcpy(m_AllocInfo.pMappedData, memory, static_cast<size_t>(size));
}

void Buffer::Transfer(Buffer* dstBuffer)
{
    VkCommandBuffer cmdBuffer = m_Context->BeginSingleTimeCommands(Context::CommandType::TRANSFER);

    VkBufferCopy copyRegion{};
    copyRegion.size = dstBuffer->m_AllocInfo.size;

    vkCmdCopyBuffer(cmdBuffer, m_Buffer, dstBuffer->m_Buffer, 1, &copyRegion);

    m_Context->EndSingleTimeCommands(Context::CommandType::TRANSFER, cmdBuffer);
}

Buffer::~Buffer()
{
//    vkDeviceWaitIdle(m_Context->GetLogicalDevice());
    if(m_Buffer)
    {
        vmaDestroyBuffer(m_Context->GetAllocator(), m_Buffer, m_Allocation);
    }
}
