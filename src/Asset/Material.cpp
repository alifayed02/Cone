#include "Core/CnPch.hpp"
#include "Material.hpp"

#include "Renderer/Context.hpp"
#include "Texture.hpp"

Material::Material(Context* context, const Material::MaterialInfo& matInfo)
        :   m_Context{context}, m_AlbedoTexture{matInfo.albedo}, m_Name{matInfo.name},
            m_DescriptorSet{}, m_DescriptorPool{}, m_DescriptorSetLayout{}
{
    CreateDescriptorPool();
    CreateDescriptorSet();
    SetSamplerData();
}

void Material::CreateDescriptorPool()
{
    // Pool will hold {1} Sampler
    VkDescriptorPoolSize poolSize{};
    poolSize.type               = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    poolSize.descriptorCount    = 1;

    // Pool will hold 1 Descriptor Set
    VkDescriptorPoolCreateInfo poolCreateInfo{};
    poolCreateInfo.sType            = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    poolCreateInfo.maxSets          = 1;
    poolCreateInfo.poolSizeCount    = 1;
    poolCreateInfo.pPoolSizes       = &poolSize;

    VK_CHECK(vkCreateDescriptorPool(m_Context->GetLogicalDevice(), &poolCreateInfo, nullptr, &m_DescriptorPool))
}

void Material::CreateDescriptorSet()
{
    VkDescriptorSetLayoutBinding layoutBinding{};
    layoutBinding.binding               = 0;
    layoutBinding.descriptorType        = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    layoutBinding.descriptorCount       = 1;
    layoutBinding.stageFlags            = VK_SHADER_STAGE_FRAGMENT_BIT;

    VkDescriptorSetLayoutCreateInfo layoutInfo{};
    layoutInfo.sType        = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    layoutInfo.bindingCount = 1;
    layoutInfo.pBindings    = &layoutBinding;

    vkCreateDescriptorSetLayout(m_Context->GetLogicalDevice(), &layoutInfo, nullptr, &m_DescriptorSetLayout);

    VkDescriptorSetAllocateInfo setAllocateInfo{};
    setAllocateInfo.sType               = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    setAllocateInfo.descriptorPool      = m_DescriptorPool;
    setAllocateInfo.descriptorSetCount  = 1;
    setAllocateInfo.pSetLayouts         = &m_DescriptorSetLayout;

    VK_CHECK(vkAllocateDescriptorSets(m_Context->GetLogicalDevice(), &setAllocateInfo, &m_DescriptorSet))
}

void Material::SetSamplerData()
{
    VkDescriptorImageInfo albedoImageInfo{};
    albedoImageInfo.sampler     = m_AlbedoTexture->GetSampler();
    albedoImageInfo.imageView   = m_AlbedoTexture->GetImage()->GetImageView();
    albedoImageInfo.imageLayout = m_AlbedoTexture->GetImage()->GetImageLayout();

    VkWriteDescriptorSet writeSet{};
    writeSet.sType              = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    writeSet.dstSet             = m_DescriptorSet;
    writeSet.dstBinding         = 0;
    writeSet.dstArrayElement    = 0;
    writeSet.descriptorCount    = 1;
    writeSet.descriptorType     = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    writeSet.pImageInfo         = &albedoImageInfo;

    vkUpdateDescriptorSets(m_Context->GetLogicalDevice(), 1, &writeSet, 0, nullptr);
}

Material::~Material()
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