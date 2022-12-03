#include "Core/CnPch.hpp"
#include "AssetManager.hpp"

#define CGLTF_IMPLEMENTATION
#include "cgltf/cgltf.h"

#include "glm/glm.hpp"

AssetManager::AssetManager(Context *context)
    :   m_Context{context}
{
}

void AssetManager::LoadMesh(std::string_view name, std::string_view path)
{
    if(m_Meshes.contains(name.data()))
    {
        return;
    }

    std::unique_ptr<Mesh> mesh = std::make_unique<Mesh>(name, path);

    std::filesystem::path cwd = std::filesystem::current_path().parent_path();
    std::string gltfPath = cwd.string() + path.data();

    cgltf_options options{};
    cgltf_data* data{};

    cgltf_result parseResult = cgltf_parse_file(&options, gltfPath.c_str(), &data);
    cgltf_result validateResult = cgltf_validate(data);

    if (parseResult == cgltf_result_success && validateResult == cgltf_result_success)
    {
        mesh->m_SubMeshes.reserve(GetSubMeshCount(data));
        LoadBuffers(gltfPath, data);
        for(size_t i = 0; i < data->meshes_count; i++)
        {
            for(size_t j = 0; j < data->meshes[i].primitives_count; j++)
            {
                std::vector<Vertex> vertices;
                std::vector<uint32_t> indices;

                LoadVertices(&data->meshes[i].primitives[j], vertices);
                LoadIndices(&data->meshes[i].primitives[j], indices);
                Material* material = LoadMaterial(name, &data->meshes[i].primitives[j]);

                SubMesh::MeshInfo meshInfo{};
                meshInfo.vertices   = vertices;
                meshInfo.indices    = indices;
                meshInfo.material   = material;

                mesh->m_SubMeshes.emplace_back(m_Context, meshInfo);
            }
        }

        cgltf_free(data);
    }

    m_Meshes[name.data()] = std::move(mesh);
}

size_t AssetManager::GetSubMeshCount(cgltf_data* data)
{
    size_t count{};
    for(size_t i = 0; i < data->meshes_count; i++)
    {
        for (size_t j = 0; j < data->meshes[i].primitives_count; j++)
        {
            count++;
        }
    }

    return count;
}

void AssetManager::LoadBuffers(std::string_view path, cgltf_data* data)
{
    cgltf_options options{};
    cgltf_load_buffers(&options, data, path.data());
}

void AssetManager::LoadVertices(cgltf_primitive* primitive, std::vector<Vertex>& vertices)
{
    if(primitive->attributes_count == 0)
    {
        return;
    }

    size_t vertexCount = primitive->attributes[0].data->count;
    vertices.resize(vertexCount);

    for(size_t i = 0; i < primitive->attributes_count; i++)
    {
        if(std::strcmp(primitive->attributes[i].name, "POSITION") == 0)
        {
            for(size_t j = 0; j < vertexCount; j++)
            {
                cgltf_float position[3];
                cgltf_accessor_read_float(primitive->attributes[i].data, j, position, sizeof(position));
                vertices[j].pos = glm::vec3(position[0], position[1], position[2]);
            }
        } else if(std::strcmp(primitive->attributes[i].name, "NORMAL") == 0)
        {
            for(size_t j = 0; j < vertexCount; j++)
            {
                cgltf_float normal[3];
                cgltf_accessor_read_float(primitive->attributes[i].data, j, normal, sizeof(normal));
                vertices[j].normal = glm::vec3(normal[0], normal[1], normal[2]);
            }
        } else if(std::strcmp(primitive->attributes[i].name, "TANGENT") == 0)
        {
            for(size_t j = 0; j < vertexCount; j++)
            {
                cgltf_float tangent[4];
                cgltf_accessor_read_float(primitive->attributes[i].data, j, tangent, sizeof(tangent));
                vertices[j].tangent = glm::vec4(tangent[0], tangent[1], tangent[2], tangent[3]);
            }
        }
        else if(std::strcmp(primitive->attributes[i].name, "TEXCOORD_0") == 0)
        {
            for(size_t j = 0; j < vertexCount; j++)
            {
                cgltf_float texCoord[2];
                cgltf_accessor_read_float(primitive->attributes[i].data, j, texCoord, sizeof(texCoord));
                vertices[j].texCoord = glm::vec2(texCoord[0], texCoord[1]);
            }
        }
    }
}

void AssetManager::LoadIndices(cgltf_primitive* primitive, std::vector<uint32_t>& indices)
{
    if(primitive->indices == nullptr)
    {
        return;
    }

    size_t indicesCount = primitive->indices->count;
    indices.reserve(indicesCount);

    for(size_t i = 0; i < indicesCount; i++)
    {
        indices.push_back(cgltf_accessor_read_index(primitive->indices, i));
    }
}

Material* AssetManager::LoadMaterial(std::string_view meshName, cgltf_primitive* primitive)
{
    if(primitive->material == nullptr)
    {
        return nullptr; // return default material
    }
    if(m_Materials.contains(primitive->material->name))
    {
        return m_Materials.at(primitive->material->name).get();
    }

    std::string materialName = primitive->material->name;

    std::filesystem::path cwd = std::filesystem::current_path().parent_path();
    std::string fullPath = cwd.string() + "/Assets/Models/" + std::string(meshName) + "/";

    Texture* albedoTexture              = nullptr;
    Texture* normalTexture              = nullptr;
    Texture* metallicRoughnessTexture   = nullptr;

    Material::MaterialObject matObject{};

    if(primitive->material->pbr_metallic_roughness.base_color_texture.texture == nullptr)
    {
        albedoTexture = LoadDefaultTexture();
    } else
    {
        std::string albedoName = primitive->material->pbr_metallic_roughness.base_color_texture.texture->image->uri;
        std::string albedoPath = fullPath + albedoName;
        albedoTexture = LoadTexture(albedoName, albedoPath);

        cgltf_float* baseColor = primitive->material->pbr_metallic_roughness.base_color_factor;
        matObject.albedoColor = glm::vec4(baseColor[0], baseColor[1], baseColor[2], baseColor[3]);
    }

    if(primitive->material->normal_texture.texture == nullptr)
    {
        normalTexture = LoadDefaultTexture();
    } else
    {
        std::string normalName = primitive->material->normal_texture.texture->image->uri;
        std::string normalPath = fullPath + normalName;
        normalTexture = LoadTexture(normalName, normalPath);
    }

    if(primitive->material->pbr_metallic_roughness.metallic_roughness_texture.texture == nullptr)
    {
        metallicRoughnessTexture = LoadDefaultTexture();
    } else
    {
        std::string metallicRoughnessName = primitive->material->pbr_metallic_roughness.metallic_roughness_texture.texture->image->uri;
        std::string metallicRoughnessPath = fullPath + metallicRoughnessName;
        metallicRoughnessTexture = LoadTexture(metallicRoughnessName, metallicRoughnessPath);

        matObject.metallicFactor    = primitive->material->pbr_metallic_roughness.metallic_factor;
        matObject.roughnessFactor   = primitive->material->pbr_metallic_roughness.roughness_factor;
    }

    // Create Material
    Material::MaterialInfo matInfo{};
    matInfo.name                = materialName;
    matInfo.albedo              = albedoTexture;
    matInfo.normal              = normalTexture;
    matInfo.metallicRoughness   = metallicRoughnessTexture;
    matInfo.materialObject      = matObject;

    m_Materials[matInfo.name] = std::make_unique<Material>(m_Context, matInfo);

    return m_Materials.at(matInfo.name).get();
}

Texture* AssetManager::LoadTexture(std::string_view name, std::string_view path)
{
    if(m_Textures.contains(name.data()))
    {
        return m_Textures.at(name.data()).get();
    }

    m_Textures[name.data()] = std::make_unique<Texture>(m_Context, name, path);
    return m_Textures.at(name.data()).get();
}

Texture* AssetManager::LoadDefaultTexture()
{
    if(!m_Textures.contains("DefaultTexture"))
    {
        std::filesystem::path cwd = std::filesystem::current_path().parent_path();
        std::string fullPath = cwd.string() + "/Assets/Textures/Black.jpeg";
        m_Textures["DefaultTexture"] = std::make_unique<Texture>(m_Context, "DefaultTexture", fullPath);
    }

    return m_Textures.at("DefaultTexture").get();
}

Mesh* AssetManager::GetMesh(std::string_view name)
{
    if(!m_Meshes.contains(name.data()))
    {
        throw std::runtime_error("Error: Mesh \"" + std::string(name.data()) + "\" does not exist!");
    }

    return m_Meshes.at(name.data()).get();
}