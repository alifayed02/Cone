#pragma once

#include "Buffer/Buffer.hpp"
#include "Image.hpp"

class Context;

class Texture
{
public:
    struct TextureInfo
    {
        int width;
        int height;
        int channels;
    };
public:
    Texture(Context* context, const std::string& path);
    ~Texture();

    Texture(const Texture& otherTexture) = delete;
    Texture& operator=(const Texture& otherTexture) = delete;
public:
    void BindDescriptorSet(VkCommandBuffer commandBuffer, VkPipelineLayout layout, uint32_t index) const;
public:
    inline VkDescriptorSetLayout GetLayout() const { return m_DescriptorSetLayout; }
private:
    void CreateImage(const std::string& path);
    void CreateDescriptorPool();
    void CreateDescriptorSet();
    void CreateSampler();
private:
    Context*                m_Context;
    std::unique_ptr<Buffer> m_StagingBuffer;
    std::unique_ptr<Image>  m_Image;
    VkSampler               m_Sampler;
    TextureInfo             m_TextureInfo;

    // Each texture should not have pool and set
    // Material class with albedo normal metallic etc texture
    // Descriptor Pool and Sets there
    VkDescriptorPool        m_DescriptorPool;
    VkDescriptorSet         m_DescriptorSet;
    VkDescriptorSetLayout   m_DescriptorSetLayout;
};
