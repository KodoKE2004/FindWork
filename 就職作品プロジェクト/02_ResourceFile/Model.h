#pragma once
#include <string>
#include <vector>
#include <memory>
#include "Mesh.h"
#include "TextureDX.h"
#include "Material.h"

class Model : public Mesh
{
public:
    Model() = default;
    bool Load(const std::string& modelFile, const std::string& texDirectory);
    const std::vector<SUBSET>& GetSubsets() const { return m_Subsets; }
    const std::vector<MATERIAL>& GetMaterials() const { return m_Materials; }
    std::vector<std::unique_ptr<Texture>> GetTextures() { return std::move(m_Textures); }
private:
    std::vector<SUBSET> m_Subsets;
    std::vector<std::unique_ptr<Texture>> m_Textures;
    std::vector<MATERIAL> m_Materials;
};