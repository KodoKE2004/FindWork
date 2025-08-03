#pragma once
#include	<assimp/Importer.hpp>
#include	<assimp/scene.h>
#include	<assimp/postprocess.h>
#include	<assimp/cimport.h>
#include	<string>
#include	<vector>
#include	<memory>
#include    "Texture.h"
#include    "Mesh.h"
#include    "Material.h"

class MeshModel : public Mesh
{
public:
    MeshModel() = default;
    bool Load(const std::string& modelFile, const std::string& texDirectory);
    const std::vector<SUBSET>&            GetSubsets()   const { return m_Subsets; }
    const std::vector<MATERIAL>&          GetMaterials() const { return m_Materials; }
    std::vector<std::shared_ptr<Texture>> GetTextures()        { return m_Textures ; }
private:
    std::vector<SUBSET> m_Subsets;
    std::vector<std::shared_ptr<Texture>> m_Textures;
    std::vector<MATERIAL> m_Materials;
};