#include "Core/CnPch.hpp"
#include "Material.hpp"

#include "Renderer/Context.hpp"
#include "Texture.hpp"

Material::Material(Context* context, const Material::MaterialInfo& matInfo)
        :   m_Context{context}, m_Name{matInfo.name}, m_AlbedoTexture{matInfo.albedo},
            m_NormalTexture{matInfo.normal}, m_MaterialObject{matInfo.materialObject},
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
    poolSize.descriptorCount    = textureCount;

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
    std::vector<VkDescriptorSetLayoutBinding> layoutBindings;
    for(size_t i = 0; i < textureCount; i++)
    {
        VkDescriptorSetLayoutBinding layoutBinding{};
        layoutBinding.binding               = i;
        layoutBinding.descriptorType        = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        layoutBinding.descriptorCount       = 1;
        layoutBinding.stageFlags            = VK_SHADER_STAGE_FRAGMENT_BIT;

        layoutBindings.push_back(layoutBinding);
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

void Material::SetSamplerData()
{
    VkDescriptorImageInfo albedoImageInfo{};
    albedoImageInfo.sampler     = m_AlbedoTexture->GetSampler();
    albedoImageInfo.imageView   = m_AlbedoTexture->GetImage()->GetImageView();
    albedoImageInfo.imageLayout = m_AlbedoTexture->GetImage()->GetImageLayout();

    VkDescriptorImageInfo normalImageInfo{};
    normalImageInfo.sampler     = m_NormalTexture->GetSampler();
    normalImageInfo.imageView   = m_NormalTexture->GetImage()->GetImageView();
    normalImageInfo.imageLayout = m_NormalTexture->GetImage()->GetImageLayout();

    VkWriteDescriptorSet albedoWriteSet{};
    albedoWriteSet.sType              = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    albedoWriteSet.dstSet             = m_DescriptorSet;
    albedoWriteSet.dstBinding         = 0;
    albedoWriteSet.dstArrayElement    = 0;
    albedoWriteSet.descriptorCount    = 1;
    albedoWriteSet.descriptorType     = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    albedoWriteSet.pImageInfo         = &albedoImageInfo;

    VkWriteDescriptorSet normalWriteSet{};
    normalWriteSet.sType              = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    normalWriteSet.dstSet             = m_DescriptorSet;
    normalWriteSet.dstBinding         = 1;
    normalWriteSet.dstArrayElement    = 0;
    normalWriteSet.descriptorCount    = 1;
    normalWriteSet.descriptorType     = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    normalWriteSet.pImageInfo         = &normalImageInfo;

    vkUpdateDescriptorSets(m_Context->GetLogicalDevice(), 1, &albedoWriteSet, 0, nullptr);
    vkUpdateDescriptorSets(m_Context->GetLogicalDevice(), 1, &normalWriteSet, 0, nullptr);
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