#include "ModelObject.h"
#include "Renderer.h"
#include "Material.h"

ModelObject::ModelObject(Camera* cam)
    : Object(cam)
{
}

void ModelObject::Initialize()
{
    if (m_ModelFile == "Null Data")
    {
        m_ModelFile = "01_AssetFile/Model/Akai/Akai.fbx";
    }
    if (m_TexDirectory == "Null Data")
    {
        m_TexDirectory = "01_AssetFile/Model/Akai";
    }
    //Meshを読み込む
    std::string tmpStr1(reinterpret_cast<const char*>(m_ModelFile.c_str()), m_ModelFile.size());
    m_Model.Load(tmpStr1, m_TexDirectory);

    m_MeshRenderer.Init(m_Model);

    m_VertexBuffer.Create(m_Model.GetVertices());
    m_IndexBuffer.Create(m_Model.GetIndices());

    m_Shader.Create("02_ResourceFile/ShaderFile/VS_Default.hlsl",
                    "02_ResourceFile/ShaderFile/PS_Default.hlsl");

    m_Subsets  = m_Model.GetSubsets();
    m_Textures = m_Model.GetTextures();

    //カメラの設定を指定
    m_Camera->SetCamera(CAMERA_3D);
    for (const auto& mtrl : m_Model.GetMaterials())
    {
        auto mat = std::make_unique<Material>();
        mat->Create(mtrl);
        m_Materials.emplace_back(std::move(mat));
    }


}

void ModelObject::Update()
{
}

void ModelObject::Draw()
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

void ModelObject::Finalize()
{
}