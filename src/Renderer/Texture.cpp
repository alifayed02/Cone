#include "Core/CnPch.hpp"
#include "Texture.hpp"

#include "Context.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"

Texture::Texture(Context* context, const std::string& path)
    :   m_Context{context}, m_Sampler{}, m_TextureInfo{},
        m_DescriptorPool{}, m_DescriptorSet{}, m_DescriptorSetLayout{}
{
    CreateImage(path);
    CreateDescriptorPool();
    CreateDescriptorSet();
    CreateSampler();
}

void Texture::CreateImage(const std::string& path)
{
    stbi_uc* pixels = stbi_load(path.c_str(), &m_TextureInfo.width, &m_TextureInfo.height, &m_TextureInfo.channels, STBI_rgb_alpha);

    if(!pixels)
    {
        throw std::runtime_error("Error: Failed to load image from " + path);
    }

    VkDeviceSize imageSize = m_TextureInfo.width * m_TextureInfo.height * 4;

    Buffer::BufferInfo stagingBufferInfo{};
    stagingBufferInfo.size              = imageSize;
    stagingBufferInfo.usageFlags        = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
    stagingBufferInfo.vmaMemoryUsage    = VMA_MEMORY_USAGE_AUTO_PREFER_HOST;
    stagingBufferInfo.vmaAllocFlags     = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT | VMA_ALLOCATION_CREATE_MAPPED_BIT;

    m_StagingBuffer = std::make_unique<Buffer>(m_Context, stagingBufferInfo);
    m_StagingBuffer->Map(pixels, imageSize);

    stbi_image_free(pixels);

    Image::ImageInfo texImageInfo{};
    texImageInfo.format         = VK_FORMAT_R8G8B8A8_UNORM;
    texImageInfo.initialLayout  = VK_IMAGE_LAYOUT_UNDEFINED;
    texImageInfo.dimension      = {(uint32_t)m_TextureInfo.width, (uint32_t)m_TextureInfo.height};
    texImageInfo.usageFlags     = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
    texImageInfo.aspectFlags    = VK_IMAGE_ASPECT_COLOR_BIT;

    m_Image = std::make_unique<Image>(m_Context, texImageInfo);

    m_Image->ChangeLayout(VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
    m_Image->CopyDataToImage(m_StagingBuffer.get(), {(uint32_t)m_TextureInfo.width, (uint32_t)m_TextureInfo.height, 1U});
    m_Image->ChangeLayout(VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
}

void Texture::CreateDescriptorPool()
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

void Texture::CreateDescriptorSet()
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

void Texture::CreateSampler()
{
    VkSamplerCreateInfo samplerCreateInfo{};
    samplerCreateInfo.sType             = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
    samplerCreateInfo.magFilter         = VK_FILTER_LINEAR;
    samplerCreateInfo.minFilter         = VK_FILTER_LINEAR;
    samplerCreateInfo.addressModeU      = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    samplerCreateInfo.addressModeV      = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    samplerCreateInfo.addressModeW      = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    samplerCreateInfo.anisotropyEnable  = VK_FALSE;

    VkPhysicalDeviceProperties properties{};
    vkGetPhysicalDeviceProperties(m_Context->GetPhysicalDevice(), &properties);

    samplerCreateInfo.maxAnisotropy             = properties.limits.maxSamplerAnisotropy;
    samplerCreateInfo.borderColor               = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
    samplerCreateInfo.unnormalizedCoordinates   = VK_FALSE;
    samplerCreateInfo.compareEnable             = VK_FALSE;
    samplerCreateInfo.compareOp                 = VK_COMPARE_OP_ALWAYS;
    samplerCreateInfo.mipmapMode                = VK_SAMPLER_MIPMAP_MODE_LINEAR;
    samplerCreateInfo.mipLodBias                = 0.0f;
    samplerCreateInfo.minLod                    = 0.0f;
    samplerCreateInfo.maxLod                    = 0.0f;

    VK_CHECK(vkCreateSampler(m_Context->GetLogicalDevice(), &samplerCreateInfo, nullptr, &m_Sampler))

    VkDescriptorImageInfo descriptorImageInfo{};
    descriptorImageInfo.sampler     = m_Sampler;
    descriptorImageInfo.imageView   = m_Image->GetImageView();
    descriptorImageInfo.imageLayout = m_Image->GetImageLayout();

    VkWriteDescriptorSet writeSet{};
    writeSet.sType              = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    writeSet.dstSet             = m_DescriptorSet;
    writeSet.dstBinding         = 0;
    writeSet.dstArrayElement    = 0;
    writeSet.descriptorCount    = 1;
    writeSet.descriptorType     = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    writeSet.pImageInfo         = &descriptorImageInfo;

    vkUpdateDescriptorSets(m_Context->GetLogicalDevice(), 1, &writeSet, 0, nullptr);
}

void Texture::BindDescriptorSet(VkCommandBuffer commandBuffer, VkPipelineLayout layout, const uint32_t index) const
{
    vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, layout, index, 1U, &m_DescriptorSet, 0U, nullptr);
}

Texture::~Texture()
{
    if(m_Sampler)
    {
        vkDestroySampler(m_Context->GetLogicalDevice(), m_Sampler, nullptr);
    }
    if(m_DescriptorSetLayout)
    {
        vkDestroyDescriptorSetLayout(m_Context->GetLogicalDevice(), m_DescriptorSetLayout, nullptr);
    }
    if(m_DescriptorPool)
    {
        vkDestroyDescriptorPool(m_Context->GetLogicalDevice(), m_DescriptorPool, nullptr);
    }
}