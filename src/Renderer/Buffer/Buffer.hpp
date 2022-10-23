#pragma once

class Context;

class Buffer
{
public:
    struct BufferInfo
    {
        VkDeviceSize                size;
        VkBufferUsageFlags          usageFlags;
        VmaAllocationCreateFlags    vmaAllocFlags;
    };
public:
    Buffer(Context* context, const BufferInfo& bufferInfo);
    ~Buffer();

    Buffer(const Buffer& otherBuffer) = delete;
    Buffer& operator=(const Buffer& otherBuffer) = delete;
public:
    inline const VkBuffer& GetBuffer() const { return m_Buffer; }
public:
    void Map(const void* memory, VkDeviceSize size) const;
    void Transfer(Buffer* dstBuffer);
private:
    Context*            m_Context;
    VkBuffer            m_Buffer;
    VmaAllocation       m_Allocation;
    VmaAllocationInfo   m_AllocInfo;
};
