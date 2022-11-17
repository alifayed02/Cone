#pragma once

class Context;

class Buffer
{
public:
    struct BufferInfo
    {
        VkDeviceSize                size;
        VkBufferUsageFlags          usageFlags;
        VmaMemoryUsage              vmaMemoryUsage;
        VmaAllocationCreateFlags    vmaAllocFlags;
    };
public:
    Buffer(Context* context, const BufferInfo& bufferInfo);
    ~Buffer();

    Buffer(Buffer&& otherBuffer) = default;

    Buffer(const Buffer& otherBuffer) = delete;
    Buffer& operator=(const Buffer& otherBuffer) = delete;
public:
    inline const VkBuffer& GetBuffer() const { return m_Buffer; }
    inline VkDeviceSize GetSize() const { return m_AllocInfo.size; }
    inline VkDeviceSize GetOffset() const { return m_AllocInfo.offset; }
public:
    void Map(const void* memory, VkDeviceSize size) const;
    void Transfer(Buffer* dstBuffer);
private:
    Context*            m_Context;
    VkBuffer            m_Buffer;
    VmaAllocation       m_Allocation;
    VmaAllocationInfo   m_AllocInfo;
};
