#include "Model.h"
#include "../Game.h"
#include "../Renderer.h"
#include "../Material.h"
#include <iostream>
Model::Model(Camera* cam) : Object(cam){}

void Model::Initialize()
{
    

    //Meshを読み込む
    m_MeshModel.mesh = std::make_shared<MeshModel>();
    GetMeshModel("NULL");

    auto shaderManager = Game::GetInstance().GetShaderManager();
    m_Shaders = {
        shaderManager->GetShader("VS_Default"),
        shaderManager->GetShader("PS_Default"),
    };
    //カメラの設定を指定
    m_Camera->SetCamera(CAMERA_3D);
    for (const auto& mtrl : m_MeshModel.mesh->GetMaterials())
    {
        auto mat = std::make_unique<Material>();
        mat->Create(mtrl);
        m_Materials.emplace_back(std::move(mat));
    }


}

void Model::Update()
{
    
}

void Model::Draw()
{
    using namespace DirectX::SimpleMath;

    Matrix r = Matrix::CreateFromYawPitchRoll(m_Rotation.y, m_Rotation.x, m_Rotation.z);
    Matrix t = Matrix::CreateTranslation(m_Position.x, m_Position.y, m_Position.z);
    Matrix s = Matrix::CreateScale(m_Scale.x, m_Scale.y, m_Scale.z);
    Matrix world = s * r * t;
    Renderer::SetWorldMatrix(&world);

    for (auto* shader : m_Shaders) {
        if (shader) {
            shader->SetGPU();
        }
    }
    // インデックスバッファ・頂点バッファをセット
    m_MeshRenderer.BeforeDraw();

    //マテリアル数分ループ 
    for (int i = 0; i < m_Subsets.size(); i++)
    {
        // マテリアルをセット(サブセット情報の中にあるマテリアルインデックスを使用)
        m_Materials[m_Subsets[i].MaterialIdx]->SetGPU();

        if (m_Materials[m_Subsets[i].MaterialIdx]->isTextureEnable())
        {
            m_Textures[m_Subsets[i].MaterialIdx]->SetGPU();
        }

        m_MeshRenderer.DrawSubset(
            m_Subsets[i].IndexNum,		// 描画するインデックス数
            m_Subsets[i].IndexBase,		// 最初のインデックスバッファの位置	
            m_Subsets[i].VertexBase);	// 頂点バッファの最初から使用
    }
}

void Model::Finalize()
{
}



void Model::GetMeshModel(std::string modelName)
{
    if (m_MeshName == modelName)
    {
        Debug::Log("現在所持しているモデルです : " + modelName);
        return;
    }

    std::cout << "Modelの取得処理開始 : " << modelName << std::endl;

    m_MeshModel.mesh = Game::GetInstance().GetMeshManager()->GetMeshModel(modelName);

    m_MeshRenderer.Init(*m_MeshModel.mesh.get());
    m_VertexBuffer.Create(m_MeshModel.mesh->GetVertices());
    m_IndexBuffer.Create (m_MeshModel.mesh->GetIndices());

    m_Subsets  = m_MeshModel.mesh->GetSubsets();
    m_Textures = m_MeshModel.mesh->GetTextures();

    m_Materials.clear();
    for (const auto& mtrl : m_MeshModel.mesh->GetMaterials())
    {
        auto mat = std::make_unique<Material>();
        mat->Create(mtrl);
        m_Materials.emplace_back(std::move(mat));
    }
}
