#pragma once

#include "glm/glm.hpp"

class Context;
class Texture;

class Material
{
public:
    struct MaterialObject
    {
        glm::vec4   albedoColor{1.0f};
        float       metallicFactor{1.0f};
        float       roughnessFactor{1.0f};
    };
    struct MaterialInfo
    {
        std::string     name;
        Texture*        albedo;
        Texture*        normal;
        Texture*        metallicRoughness;
        MaterialObject  materialObject;
    };
public:
    Material(Context* context, const MaterialInfo& matInfo);
    ~Material();

    Material(const Material& otherMaterial) = delete;
    Material& operator=(const Material& otherMaterial) = delete;
public:
    inline VkDescriptorSet GetDescriptorSet() const { return m_DescriptorSet; }
    inline VkDescriptorSetLayout GetLayout() const { return m_DescriptorSetLayout; }
    inline const MaterialObject& GetMaterialObject() const { return m_MaterialObject; }
private:
    void CreateDescriptorPool();
    void CreateDescriptorSet();
    void SetSamplerData();
private:
    const uint32_t textureCount = 3;
private:
    Context*                m_Context;
    std::string             m_Name;
    Texture*                m_AlbedoTexture;
    Texture*                m_NormalTexture;
    Texture*                m_MetallicRoughness;
    MaterialObject          m_MaterialObject;
private:
    VkDescriptorSet         m_DescriptorSet;
    VkDescriptorPool        m_DescriptorPool;
    VkDescriptorSetLayout   m_DescriptorSetLayout;
};