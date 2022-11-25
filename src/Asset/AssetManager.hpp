#pragma once

#include "Mesh.hpp"
#include "Texture.hpp"
#include "Material.hpp"

struct cgltf_data;
struct cgltf_primitive;

class Context;

class AssetManager
{
public:
    AssetManager(Context* context);
    ~AssetManager() = default;

    AssetManager(const AssetManager& otherAssetManager) = delete;
    AssetManager& operator=(const AssetManager& otherAssetManager) = delete;
public:
    void LoadMesh(std::string_view name, std::string_view path);
    Mesh* GetMesh(std::string_view name);
private:
    size_t GetSubMeshCount(cgltf_data* data);
    void LoadBuffers(std::string_view path, cgltf_data* data);
    void LoadVertices(cgltf_primitive* primitive, std::vector<Vertex>& vertices);
    void LoadIndices(cgltf_primitive* primitive, std::vector<uint32_t>& indices);
    Material* LoadMaterial(std::string_view meshName, cgltf_primitive* primitive);
    Texture* LoadTexture(std::string_view name, std::string_view path);
    Texture* LoadDefaultTexture();
private:
    Context*                                                    m_Context;
    std::unordered_map<std::string, std::unique_ptr<Mesh>>      m_Meshes;
    std::unordered_map<std::string, std::unique_ptr<Texture>>   m_Textures;
    std::unordered_map<std::string, std::unique_ptr<Material>>  m_Materials;
};