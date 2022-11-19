#include "Core/CnPch.hpp"
#include "Texture.hpp"

#include "Renderer/Context.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"

Texture::Texture(Context* context, std::string_view name, std::string_view path)
    :   m_Context{context}, m_Name{name.data()}, m_FilePath{path.data()}, m_Sampler{}, m_TextureInfo{}
{
    CreateImage(m_FilePath);
    CreateSampler();
}

void Texture::CreateImage(const std::string& path)
{
    stbi_uc* pixels = stbi_load(path.c_str(), &m_TextureInfo.width, &m_TextureInfo.height, &m_TextureInfo.channels, STBI_rgb_alpha);

    if(!pixels)
    {
        std::string errorPath = R"(/Users/alifayed/CLionProjects/Cone/Assets/Textures/blendermonkey.jpg)";
        pixels = stbi_load(errorPath.c_str(), &m_TextureInfo.width, &m_TextureInfo.height, &m_TextureInfo.channels, STBI_rgb_alpha);
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
    texImageInfo.desiredLayout  = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
    texImageInfo.dimension      = {(uint32_t)m_TextureInfo.width, (uint32_t)m_TextureInfo.height};
    texImageInfo.usageFlags     = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
    texImageInfo.aspectFlags    = VK_IMAGE_ASPECT_COLOR_BIT;

    m_Image = std::make_unique<Image>(m_Context, texImageInfo);

    m_Image->CopyDataToImage(m_StagingBuffer.get(), {(uint32_t)m_TextureInfo.width, (uint32_t)m_TextureInfo.height, 1U});
    m_Image->ChangeLayout(VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
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
    samplerCreateInfo.anisotropyEnable  = VK_TRUE;

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
}

Texture::~Texture()
{
    if(m_Sampler)
    {
        vkDestroySampler(m_Context->GetLogicalDevice(), m_Sampler, nullptr);
    }
}