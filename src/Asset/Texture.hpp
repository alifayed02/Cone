#pragma once

#include "Renderer/Buffer/Buffer.hpp"
#include "Renderer/Image.hpp"

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
    Texture(Context* context, std::string_view name, std::string_view path);
    ~Texture();

    Texture(const Texture& otherTexture) = delete;
    Texture& operator=(const Texture& otherTexture) = delete;
public:
    inline VkSampler GetSampler() const { return m_Sampler; }
    inline const Image* GetImage() const { return m_Image.get(); }
private:
    void CreateImage(const std::string& path);
    void CreateSampler();
private:
    Context*                m_Context;
    std::string             m_Name;
    std::string             m_FilePath;
    std::unique_ptr<Buffer> m_StagingBuffer;
    std::unique_ptr<Image>  m_Image;
    VkSampler               m_Sampler;
    TextureInfo             m_TextureInfo;
};