#pragma once

class Context;

class DescriptorSet
{
public:
    struct BindingInfo
    {
        VkDescriptorType        type;
        uint32_t                binding;
        VkShaderStageFlags      stageFlags;
        VkDescriptorImageInfo*  imageInfo;
        VkDescriptorBufferInfo* bufferInfo;
    };
public:
    DescriptorSet(Context* context, std::vector<BindingInfo>& bindings);
    ~DescriptorSet();

    DescriptorSet(const DescriptorSet& otherDescriptorSet) = delete;
    DescriptorSet& operator=(const DescriptorSet& otherDescriptorSet) = delete;
public:
    inline VkDescriptorSet GetDescriptorSet() const { return m_DescriptorSet; }
    inline VkDescriptorSetLayout GetDescriptorSetLayout() const { return m_DescriptorSetLayout; }
private:
    void CreateDescriptorPool();
    void CreateDescriptorSet();
    void UpdateDescriptorSet();
private:
    Context*                m_Context;
    VkDescriptorSet         m_DescriptorSet;
    VkDescriptorSetLayout   m_DescriptorSetLayout;
    VkDescriptorPool        m_DescriptorPool;
private:
    std::vector<BindingInfo>* m_Bindings;
};
