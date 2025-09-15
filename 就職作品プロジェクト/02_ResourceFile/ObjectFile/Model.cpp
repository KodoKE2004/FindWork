#include "Model.h"
#include "../Game.h"
#include "../Renderer.h"
#include "../Material.h"
#include "../Debug.hpp"
#include <iostream>

Model::Model(Camera* cam) : Object(cam) {}

void Model::Initialize()
{
    m_MeshModel.mesh = std::make_shared<MeshModel>();
    GetMeshModel("NULL");

    auto shaderManager = Game::GetInstance().GetShaderManager();
    m_Shaders = {
        shaderManager->GetShader("VS_Default"),
        shaderManager->GetShader("PS_Default"),
    };

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
    if (m_IsSky) {
        DrawSky();
        return;
    }

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
    m_MeshRenderer.BeforeDraw();

    for (int i = 0; i < m_Subsets.size(); i++)
    {
        m_Materials[m_Subsets[i].MaterialIdx]->SetGPU();

        if (m_Materials[m_Subsets[i].MaterialIdx]->isTextureEnable())
        {
            m_Textures[m_Subsets[i].MaterialIdx]->SetGPU();
        }

        m_MeshRenderer.DrawSubset(
            m_Subsets[i].IndexNum,
            m_Subsets[i].IndexBase,
            m_Subsets[i].VertexBase);
    }
}

void Model::Finalize()
{
}

void Model::GetMeshModel(std::string modelName)
{
    if (m_MeshName == modelName)
    {
        Debug::Log("already loaded : " + modelName);
        return;
    }

    std::cout << "Model load start : " << modelName << std::endl;

    m_MeshModel.mesh = Game::GetInstance().GetMeshManager()->GetMeshModel(modelName);

    m_MeshRenderer.Init(*m_MeshModel.mesh.get());
    m_VertexBuffer.Create(m_MeshModel.mesh->GetVertices());
    m_IndexBuffer.Create(m_MeshModel.mesh->GetIndices());

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

void Model::EnableSkyDome(float radius)
{
    m_IsSky = true;
    m_SkyRadius = radius;
    CreateSkyStates();
}

void Model::DisableSkyDome()
{
    m_IsSky = false;
}

void Model::CreateSkyStates()
{
    auto dev = Renderer::GetDevice();

    D3D11_RASTERIZER_DESC rs{};
    rs.FillMode        = D3D11_FILL_SOLID;
    rs.CullMode        = D3D11_CULL_FRONT;
    rs.DepthClipEnable = TRUE;
    dev->CreateRasterizerState(&rs, m_RS_CullFront.GetAddressOf());

    D3D11_DEPTH_STENCIL_DESC dss{};
    dss.DepthEnable   = TRUE;
    dss.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
    dss.DepthFunc     = D3D11_COMPARISON_LESS_EQUAL;
    dss.StencilEnable = FALSE;
    dev->CreateDepthStencilState(&dss, m_DSS_NoWrite_Lequal.GetAddressOf());
}

void Model::DrawSky()
{
    using namespace DirectX::SimpleMath;

    auto ctx = Renderer::GetDeviceContext();
    ID3D11RasterizerState* prevRS = nullptr;
    ctx->RSGetState(&prevRS);
    ID3D11DepthStencilState* prevDSS = nullptr;
    UINT prevRef = 0;
    ctx->OMGetDepthStencilState(&prevDSS, &prevRef);

    ctx->RSSetState(m_RS_CullFront.Get());
    ctx->OMSetDepthStencilState(m_DSS_NoWrite_Lequal.Get(), 0);

    auto camPos = m_Camera->GetPosition();
    Matrix s = Matrix::CreateScale(m_SkyRadius);
    Matrix t = Matrix::CreateTranslation(camPos.x, camPos.y, camPos.z);
    Matrix world = s * t;
    Renderer::SetWorldMatrix(&world);

    for (auto* shader : m_Shaders) {
        if (shader) { shader->SetGPU(); }
    }

    m_MeshRenderer.BeforeDraw();

    for (int i = 0; i < m_Subsets.size(); i++)
    {
        m_Materials[m_Subsets[i].MaterialIdx]->SetGPU();
        if (m_Materials[m_Subsets[i].MaterialIdx]->isTextureEnable())
        {
            m_Textures[m_Subsets[i].MaterialIdx]->SetGPU();
        }
        m_MeshRenderer.DrawSubset(
            m_Subsets[i].IndexNum,
            m_Subsets[i].IndexBase,
            m_Subsets[i].VertexBase);
    }

    ctx->RSSetState(prevRS);
    ctx->OMSetDepthStencilState(prevDSS, prevRef);
    if (prevRS)  { prevRS->Release(); }
    if (prevDSS) { prevDSS->Release(); }
}
