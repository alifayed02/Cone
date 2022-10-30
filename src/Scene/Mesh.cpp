#include "Core/CnPch.hpp"
#include "Mesh.hpp"

#include "Renderer/Context.hpp"

Mesh::Mesh(Context* context, const Mesh::MeshInfo& meshInfo)
    :   m_Context{context}, m_VertexBuffer(context, meshInfo.vertices),
        m_IndexBuffer(context, meshInfo.indices)
{
}