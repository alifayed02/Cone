#pragma once

#include "Renderer/Buffer/VertexBuffer.hpp"
#include "Renderer/Buffer/IndexBuffer.hpp"

class Context;
class Material;

class SubMesh
{
public:
    struct MeshInfo
    {
        std::vector<Vertex>     vertices;
        std::vector<uint32_t>   indices;
        Material*               material;
    };
public:
    SubMesh(Context* context, const MeshInfo& meshInfo);
    ~SubMesh() = default;

    SubMesh(SubMesh&& otherSubMesh) = default;

    SubMesh(const SubMesh& otherMesh) = delete;
    SubMesh& operator=(const SubMesh& otherMesh) = delete;
public:
    inline const VertexBuffer& GetVertexBuffer() const { return m_VertexBuffer; }
    inline const IndexBuffer& GetIndexBuffer() const { return m_IndexBuffer; }
    inline const Material* GetMaterial() const { return m_Material; }
private:
    Context*        m_Context;
    Material*       m_Material;
    VertexBuffer    m_VertexBuffer;
    IndexBuffer     m_IndexBuffer;
};
