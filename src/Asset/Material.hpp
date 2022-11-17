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
    Context*                m_Context;
    Texture*                m_AlbedoTexture;
    std::string             m_Name;
private:
    VkDescriptorSet         m_DescriptorSet;
    VkDescriptorPool        m_DescriptorPool;
    VkDescriptorSetLayout   m_DescriptorSetLayout;
};