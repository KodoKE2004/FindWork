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

class StaticMesh : public Mesh
{
public:
    StaticMesh() = default;
    bool Load(const std::string& modelFile, const std::string& texDirectory);
    const vector<SUBSET>&            GetSubsets()   const { return m_Subsets; }
    const vector<MATERIAL>&          GetMaterials() const { return m_Materials; }
    vector<pShared<Texture>> GetTextures()        { return m_Textures ; }
private:
    vector<SUBSET>           m_Subsets;
    vector<pShared<Texture>> m_Textures;
    vector<MATERIAL>         m_Materials;
};