#pragma once

class Context;
class Texture;

class Material
{
public:
    struct MaterialInfo
    {
        std::string name;
        Texture*    albedo;
        Texture*    normal;
    };
public:
    Material(Context* context, const MaterialInfo& matInfo);
    ~Material();

    Material(const Material& otherMaterial) = delete;
    Material& operator=(const Material& otherMaterial) = delete;
public:
    inline VkDescriptorSet GetDescriptorSet() const { return m_DescriptorSet; }
    inline VkDescriptorSetLayout GetLayout() const { return m_DescriptorSetLayout; }
private:
    void CreateDescriptorPool();
    void CreateDescriptorSet();
    void SetSamplerData();
private:
    const uint32_t materialCount = 2;
private:
    Context*                m_Context;
    std::string             m_Name;
    Texture*                m_AlbedoTexture;
    Texture*                m_NormalTexture;
private:
    VkDescriptorSet         m_DescriptorSet;
    VkDescriptorPool        m_DescriptorPool;
    VkDescriptorSetLayout   m_DescriptorSetLayout;
};