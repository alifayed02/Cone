#pragma once

#include "SubMesh.hpp"

class Mesh
{
public:
    Mesh(std::string_view name, std::string_view path);
    ~Mesh() = default;

    Mesh(const Mesh& otherMesh) = delete;
    Mesh& operator=(const Mesh& otherMesh) = delete;
public:
    const std::string& GetName() const { return m_Name; }
public:
    std::vector<SubMesh>    m_SubMeshes;
private:
    std::string             m_Name;
    std::string             m_FilePath;
};