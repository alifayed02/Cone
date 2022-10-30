#pragma once

#include "Renderer/Buffer/VertexBuffer.hpp"
#include "Renderer/Buffer/IndexBuffer.hpp"
#include "Renderer/Texture.hpp"

class Context;

class Mesh
{
public:
    struct MeshInfo
    {
        std::vector<Vertex>     vertices;
        std::vector<uint16_t>   indices;
        std::string             texturePath;
    };
public:
    Mesh(Context* context, const MeshInfo& meshInfo);
    ~Mesh() = default;

    Mesh(const Mesh& otherMesh) = delete;
    Mesh& operator=(const Mesh& otherMesh) = delete;
public:
    inline const VertexBuffer& GetVertexBuffer() const { return m_VertexBuffer; }
    inline const IndexBuffer& GetIndexBuffer() const { return m_IndexBuffer; }
    inline const Texture& GetTexture() const { return m_Texture; }
private:
    Context*        m_Context;
    Texture         m_Texture;
    VertexBuffer    m_VertexBuffer;
    IndexBuffer     m_IndexBuffer;
};
