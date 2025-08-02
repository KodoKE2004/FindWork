#include "MeshModel.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <filesystem>

#pragma comment(lib, "assimp-vc143-mtd.lib")

#ifdef _DEBUG
    #include <iostream>
    #include "Debug.hpp"
#endif

bool MeshModel::Load(const std::string& modelFile, const std::string& texDirectory)
{
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(modelFile,
        aiProcess_Triangulate |
        aiProcess_ConvertToLeftHanded);
    if (!scene) return false;

    m_Vertices.clear();
    m_Indices.clear();
    m_Subsets.clear();
    m_Textures.clear();
    m_Materials.clear();

    // Materials and textures
    m_Textures.resize(scene->mNumMaterials);
    m_Materials.resize(scene->mNumMaterials);

    for (unsigned int m = 0; m < scene->mNumMaterials; ++m)
    {
        aiMaterial* material = scene->mMaterials[m];
        aiString path;
        if (material->GetTexture(aiTextureType_DIFFUSE, 0, &path) == AI_SUCCESS)
        {
            std::filesystem::path file = std::filesystem::path(path.C_Str()).filename();
            std::filesystem::path texPath = std::filesystem::path(texDirectory) / file;  
            m_Textures[m] = std::make_unique<Texture>();
            m_Textures[m]->LoadFromFile(texPath.string());
            m_Materials[m].TextureEnable = TRUE;
        }
        else
        {
            m_Materials[m].TextureEnable = FALSE;
        }

        aiColor4D ambient{ 0,0,0,1 };
        aiColor4D diffuse{ 1,1,1,1 };
        aiColor4D specular{ 0,0,0,1 };
        aiColor4D emission{ 0,0,0,1 };
        float shininess = 0.f;

        aiGetMaterialColor(material, AI_MATKEY_COLOR_AMBIENT, &ambient);
        aiGetMaterialColor(material, AI_MATKEY_COLOR_DIFFUSE, &diffuse);
        aiGetMaterialColor(material, AI_MATKEY_COLOR_SPECULAR, &specular);
        aiGetMaterialColor(material, AI_MATKEY_COLOR_EMISSIVE, &emission);
        aiGetMaterialFloat(material, AI_MATKEY_SHININESS, &shininess);

        m_Materials[m].Ambient  = DirectX::SimpleMath::Color(ambient.r, ambient.g, ambient.b, ambient.a);
        m_Materials[m].Diffuse  = DirectX::SimpleMath::Color(diffuse.r, diffuse.g, diffuse.b, diffuse.a);
        m_Materials[m].Specular = DirectX::SimpleMath::Color(specular.r, specular.g, specular.b, specular.a);
        m_Materials[m].Emission = DirectX::SimpleMath::Color(emission.r, emission.g, emission.b, emission.a);
        m_Materials[m].Shiness  = shininess;
    }

    // meshes
    for (unsigned int m = 0; m < scene->mNumMeshes; ++m)
    {
        aiMesh* mesh = scene->mMeshes[m];
        SUBSET sub{};
        sub.VertexBase = static_cast<unsigned int>(m_Vertices.size());
        sub.IndexBase = static_cast<unsigned int>(m_Indices.size());
        sub.VertexNum = mesh->mNumVertices;
        sub.IndexNum = mesh->mNumFaces * 3;
        sub.MaterialIdx = mesh->mMaterialIndex;
        m_Subsets.push_back(sub);

        for (unsigned int v = 0; v < mesh->mNumVertices; ++v)
        {
            VERTEX_3D vert{};
            vert.position = NVector3(mesh->mVertices[v].x, mesh->mVertices[v].y, mesh->mVertices[v].z);
            vert.normal = NVector3(mesh->mNormals ? mesh->mNormals[v].x : 0.f,
                                   mesh->mNormals ? mesh->mNormals[v].y : 0.f,
                                   mesh->mNormals ? mesh->mNormals[v].z : 0.f);
            if (mesh->HasTextureCoords(0))
                vert.uv = DirectX::SimpleMath::Vector2(mesh->mTextureCoords[0][v].x, mesh->mTextureCoords[0][v].y);
            else
                vert.uv = DirectX::SimpleMath::Vector2(0.f, 0.f);
            vert.color = DirectX::SimpleMath::Color(1.f, 1.f, 1.f, 1.f);
            m_Vertices.push_back(vert);
        }

        for (unsigned int f = 0; f < mesh->mNumFaces; ++f)
        {
            const aiFace& face = mesh->mFaces[f];
            if (face.mNumIndices == 3)
            {
                m_Indices.push_back(face.mIndices[0]);
                m_Indices.push_back(face.mIndices[1]);
                m_Indices.push_back(face.mIndices[2]);
            }
        }
    }

    std::cout << "ƒ‚ƒfƒ‹“Ç‚Ýž‚ÝŠ®—¹" << std::endl;
    return true;
}