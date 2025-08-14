#pragma once
#include "Object.h"
#include "../MeshModel.h"
#include "../MeshRenderer.h"
#include "../VertexBuffer.h"
#include "../IndexBuffer.h"
#include "../Material.h"
#include <memory>
#include <vector>


struct MeshInfo {
    std::shared_ptr<MeshModel> mesh;
    std::string filePath;
    std::string texDirectory;
};

/// <summary>
/// オブジェクトモデル基底クラス
/// </summary>
class Model : public Object
{
protected:
    std::string  m_MeshName;
    MeshInfo     m_MeshModel;
    MeshRenderer m_MeshRenderer;

    VertexBuffer<VERTEX_3D> m_VertexBuffer;
    IndexBuffer             m_IndexBuffer;
    std::vector<SUBSET>     m_Subsets;
    std::vector<std::shared_ptr<Texture>> m_Textures;
    std::vector<std::unique_ptr<Material>> m_Materials;

public:
    Model(Camera* cam);

    void Initialize() override;
    void Update()     override;
    void Draw()       override;
    void Finalize()   override;

    void GetMeshModel(std::string modelName);
};

