#pragma once
#include    "Material.h"
#include    "Mesh.h"
#include    "Texture.h"
#include	<assimp/cimport.h>
#include	<assimp/Importer.hpp>
#include	<assimp/postprocess.h>
#include	<assimp/scene.h>
#include	<memory>
#include	<string>
#include	<vector>

class StaticMesh : public Mesh
{
public:
    StaticMesh() = default;
    bool Load(const std::string& modelFile, const std::string& texDirectory);
    const std::vector<SUBSET>&            GetSubsets()   const { return m_Subsets; }
    const std::vector<MATERIAL>&          GetMaterials() const { return m_Materials; }
    std::vector<std::shared_ptr<Texture>> GetTextures()        { return m_Textures ; }
private:
    std::vector<SUBSET> m_Subsets;
    std::vector<std::shared_ptr<Texture>> m_Textures;
    std::vector<MATERIAL> m_Materials;
};