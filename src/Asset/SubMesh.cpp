#include "Core/CnPch.hpp"
#include "SubMesh.hpp"

#include "Renderer/Context.hpp"

SubMesh::SubMesh(Context* context, const SubMesh::MeshInfo& meshInfo)
    :   m_Context{context}, m_Material{meshInfo.material},
        m_VertexBuffer{context, meshInfo.vertices}, m_IndexBuffer{context, meshInfo.indices}
{
}