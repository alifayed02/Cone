#include "Core/CnPch.hpp"
#include "Mesh.hpp"

Mesh::Mesh(std::string_view name, std::string_view path)
        :   m_Name{name.data()}, m_FilePath{path.data()}
{
}