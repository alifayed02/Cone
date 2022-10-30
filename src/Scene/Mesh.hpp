#pragma once

#include "Renderer/Buffer/VertexBuffer.hpp"
#include "Renderer/Buffer/IndexBuffer.hpp"

class Context;

class Mesh
{
public:
    struct MeshInfo
    {
        std::vector<Vertex>     vertices;
        std::vector<uint16_t>   indices;
    };
public:
    Mesh(Context* context, const MeshInfo& meshInfo);
    ~Mesh() = default;

    Mesh(const Mesh& otherMesh) = delete;
    Mesh& operator=(const Mesh& otherMesh) = delete;
public:
    inline const VertexBuffer& GetVertexBuffer() const { return m_VertexBuffer; }
    inline const IndexBuffer& GetIndexBuffer() const { return m_IndexBuffer; }
private:
    Context*        m_Context;
    VertexBuffer    m_VertexBuffer;
    IndexBuffer     m_IndexBuffer;
};
