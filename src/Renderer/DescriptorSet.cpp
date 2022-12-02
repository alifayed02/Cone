#include "Core/CnPch.hpp"
#include "DescriptorSet.hpp"

#include "Context.hpp"

DescriptorSet::DescriptorSet(Context* context, std::vector<BindingInfo>& bindings)
    :   m_Context{context}, m_DescriptorSet{}, m_DescriptorSetLayout{}, m_DescriptorPool{},
        m_Bindings{&bindings}
{
    CreateDescriptorPool();
    CreateDescriptorSet();
    UpdateDescriptorSet();
}

void DescriptorSet::CreateDescriptorPool()
{
    std::vector<VkDescriptorPoolSize> poolSizes;

    for(auto& binding : *m_Bindings)
    {
        VkDescriptorPoolSize poolSize{};
        poolSize.type               = binding.type;
        poolSize.descriptorCount    = 1;

        poolSizes.push_back(poolSize);
    }

    VkDescriptorPoolCreateInfo poolCreateInfo{};
    poolCreateInfo.sType            = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    poolCreateInfo.maxSets          = 1;
    poolCreateInfo.poolSizeCount    = poolSizes.size();
    poolCreateInfo.pPoolSizes       = poolSizes.data();

    VK_CHECK(vkCreateDescriptorPool(m_Context->GetLogicalDevice(), &poolCreateInfo, nullptr, &m_DescriptorPool))
}

void DescriptorSet::CreateDescriptorSet()
{
    std::vector<VkDescriptorSetLayoutBinding> layoutBindings(m_Bindings->size());

    for(size_t i = 0; auto& binding : *m_Bindings)
    {
        VkDescriptorSetLayoutBinding layoutBinding{};
        layoutBinding.binding           = binding.binding;
        layoutBinding.descriptorType    = binding.type;
        layoutBinding.descriptorCount   = 1;
        layoutBinding.stageFlags        = binding.stageFlags;

        layoutBindings[i++] = layoutBinding;
    }

    VkDescriptorSetLayoutCreateInfo layoutInfo{};
    layoutInfo.sType        = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    layoutInfo.bindingCount = layoutBindings.size();
    layoutInfo.pBindings    = layoutBindings.data();

    vkCreateDescriptorSetLayout(m_Context->GetLogicalDevice(), &layoutInfo, nullptr, &m_DescriptorSetLayout);

    VkDescriptorSetAllocateInfo setAllocateInfo{};
    setAllocateInfo.sType               = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    setAllocateInfo.descriptorPool      = m_DescriptorPool;
    setAllocateInfo.descriptorSetCount  = 1;
    setAllocateInfo.pSetLayouts         = &m_DescriptorSetLayout;

    VK_CHECK(vkAllocateDescriptorSets(m_Context->GetLogicalDevice(), &setAllocateInfo, &m_DescriptorSet))
}

void DescriptorSet::UpdateDescriptorSet()
{
    std::vector<VkWriteDescriptorSet> writeSets;
    for(auto& binding : *m_Bindings)
    {
        VkWriteDescriptorSet writeSet{};
        writeSet.sType              = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        writeSet.dstSet             = m_DescriptorSet;
        writeSet.dstBinding         = binding.binding;
        writeSet.dstArrayElement    = 0;
        writeSet.descriptorCount    = 1;
        writeSet.descriptorType     = binding.type;
        writeSet.pImageInfo         = binding.imageInfo != nullptr ? binding.imageInfo : nullptr;
        writeSet.pBufferInfo        = binding.bufferInfo != nullptr ? binding.bufferInfo : nullptr;

        writeSets.push_back(writeSet);
    }

    vkUpdateDescriptorSets(m_Context->GetLogicalDevice(), writeSets.size(), writeSets.data(), 0, nullptr);
}

DescriptorSet::~DescriptorSet()
{
    if(m_DescriptorSetLayout)
    {
        vkDestroyDescriptorSetLayout(m_Context->GetLogicalDevice(), m_DescriptorSetLayout, nullptr);
    }
    if(m_DescriptorPool)
    {
        vkDestroyDescriptorPool(m_Context->GetLogicalDevice(), m_DescriptorPool, nullptr);
    }
}
