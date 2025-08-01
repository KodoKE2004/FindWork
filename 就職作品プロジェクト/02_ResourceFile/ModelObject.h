#pragma once
#include "Object.h"
#include "Model.h"
#include "MeshRenderer.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "Material.h"
#include <memory>
#include <vector>

class ModelObject : public Object
{
private:
    std::string m_ModelFile = "Null Data";
    std::string m_TexDirectory = "Null Data";

    Model        m_Model;
    MeshRenderer m_MeshRenderer;

    VertexBuffer<VERTEX_3D> m_VertexBuffer;
    IndexBuffer             m_IndexBuffer;
    std::vector<SUBSET>     m_Subsets;
    std::vector<std::unique_ptr<Texture>> m_Textures;
    std::vector<std::unique_ptr<Material>> m_Materials;

public:




    ModelObject(Camera* cam);

    void Initialize() override;
    void Update()     override;
    void Draw()       override;
    void Finalize()   override;

    void SetModelFile(const std::string& model) { m_ModelFile = model; }
    void SetTexDirectory(const std::string& dir) { m_TexDirectory = dir; }
    void SetModel(const std::string& model,const std::string& tex)
    {
        SetModelFile   (model);
        SetTexDirectory(tex);
    }
};

