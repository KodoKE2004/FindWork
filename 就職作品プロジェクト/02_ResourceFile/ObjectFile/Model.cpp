#include "Model.h"
#include "../Game.h"
#include "../Renderer.h"
#include "../Material.h"

Model::Model(Camera* cam) : Object(cam)
{
}

void Model::Initialize()
{
    

    //Meshを読み込む
    m_MeshModel = std::make_shared<MeshModel>();
    SetMeshModel("NULL");

    m_MeshRenderer.Init(*m_MeshModel.get());

    m_VertexBuffer.Create(m_MeshModel->GetVertices());
    m_IndexBuffer .Create(m_MeshModel->GetIndices());

    m_Shader.Create("02_ResourceFile/ShaderFile/VS_Default.hlsl",
                    "02_ResourceFile/ShaderFile/PS_Default.hlsl");

    m_Subsets  = m_MeshModel->GetSubsets();
    m_Textures = m_MeshModel->GetTextures();

    //カメラの設定を指定
    m_Camera->SetCamera(CAMERA_3D);
    for (const auto& mtrl : m_MeshModel->GetMaterials())
    {
        auto mat = std::make_unique<Material>();
        mat->Create(mtrl);
        m_Materials.emplace_back(std::move(mat));
    }


}

void Model::Update(){

}

void Model::Draw()
{
    using namespace DirectX::SimpleMath;

    Matrix r = Matrix::CreateFromYawPitchRoll(m_Rotation.y, m_Rotation.x, m_Rotation.z);
    Matrix t = Matrix::CreateTranslation(m_Position.x, m_Position.y, m_Position.z);
    Matrix s = Matrix::CreateScale(m_Scale.x, m_Scale.y, m_Scale.z);
    Matrix world = s * r * t;
    Renderer::SetWorldMatrix(&world);

    m_Shader.SetGPU();

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

void Model::SetMeshModel(std::string modelName)
{
    m_MeshModel = Game::GetInstance().GetMeshManager()->GetMeshModel(modelName);
}
