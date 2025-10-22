#include "Wipe.h"

#include "Application.h"
#include "main.h"
#include "Game.h"
#include "Renderer.h"
#include <vector>

Wipe::Wipe(Camera* cam) : TransitionBase(cam)
{

}

void Wipe::Initialize()
{
    auto textureMgr = Game::GetInstance().GetTextureManager();
    m_Texture = textureMgr->GetTexture("Black.png");
    SetScale(SCREEN_WIDTH, SCREEN_HEIGHT, 1.0f);
    SetColor(0.0f, 0.0f, 0.0f, 1.0f);

    std::vector<VERTEX_3D> vertices;
    vertices.resize(4);

    vertices[0].position = NVector3(-0.5f, 0.5f, 0.0f);
    vertices[1].position = NVector3(0.5f, 0.5f, 0.0f);
    vertices[2].position = NVector3(-0.5f, -0.5f, 0.0f);
    vertices[3].position = NVector3(0.5f, -0.5f, 0.0f);

    vertices[0].color = Color(1.0f, 1.0f, 1.0f, 1.0f);
    vertices[1].color = Color(1.0f, 1.0f, 1.0f, 1.0f);
    vertices[2].color = Color(1.0f, 1.0f, 1.0f, 1.0f);
    vertices[3].color = Color(1.0f, 1.0f, 1.0f, 1.0f);

    vertices[0].uv = Vector2(0.0f, 0.0f);
    vertices[1].uv = Vector2(1.0f, 0.0f);
    vertices[2].uv = Vector2(0.0f, 1.0f);
    vertices[3].uv = Vector2(1.0f, 1.0f);

    m_VertexBuffer.Create(vertices);

    std::vector<unsigned int> indices(4);
    indices[0] = 0;
    indices[1] = 1;
    indices[2] = 2;
    indices[3] = 3;
    m_IndexBuffer.Create(indices);

    SetShader("VS_Alpha", "PS_Alpha");

    m_Materiale = std::make_unique<Material>();
    MATERIAL mtrl;
    mtrl.Diffuse = Color(1.0f, 1.0f, 1.0f, 1.0f);
    mtrl.Shiness = 1;
    mtrl.TextureEnable = true;
    m_Materiale->Create(mtrl);

    m_BaseInitialized = false;
}

void Wipe::Update()
{
}

void Wipe::Draw()
{
    Renderer::SetDepthEnable(false);
    Renderer::SetBlendState(BS_ALPHABLEND);

    Matrix r = Matrix::CreateFromYawPitchRoll(m_Rotation.x, m_Rotation.y, m_Rotation.z);
    Matrix t = Matrix::CreateTranslation(m_Position.x, m_Position.y, m_Position.z);
    Matrix s = Matrix::CreateScale(m_Scale.x, m_Scale.y, m_Scale.z);
    Matrix world = s * r * t;
    Renderer::SetWorldMatrix(&world);

    ID3D11DeviceContext* context = Renderer::GetDeviceContext();
    context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

    SetGPU();
    m_VertexBuffer.SetGPU();
    m_IndexBuffer.SetGPU();

    if (m_SnapshotSRV)
    {
        ID3D11ShaderResourceView* srv = m_SnapshotSRV.Get();
        context->PSSetShaderResources(0, 1, &srv);
    }
    else if (m_Texture)
    {
        m_Texture->SetGPU();
    }

    m_Materiale->SetDiffuse(DirectX::XMFLOAT4(m_Color.x, m_Color.y, m_Color.z, m_Color.w));
    m_Materiale->Update();
    m_Materiale->SetGPU();

    Renderer::SetUV(m_UOffset, m_VOffset, m_UScale, m_VScale);

    Camera::ScopedMode scopedMode(m_Camera, CAMERA_2D);
    context->DrawIndexed(4, 0, 0);

    if (m_SnapshotSRV)
    {
        ID3D11ShaderResourceView* nullSrv[1] = { nullptr };
        context->PSSetShaderResources(0, 1, nullSrv);
    }

    Renderer::SetDepthEnable(true);
}

void Wipe::Finalize()
{
    m_SnapshotSRV.Reset();
}

void Wipe::SetProgress(float p)
{
    m_Progress = std::clamp(p, 0.0f, 1.0f);
    UpdateLayout();
}

void Wipe::SetSnapshot(ID3D11ShaderResourceView* srv)
{
    m_SnapshotSRV = srv;
}

void Wipe::UpdateLayout()
{
    if (!m_BaseInitialized)
    {
        m_BaseScale = GetScale();
        m_BasePos   = GetPos();
        m_BaseInitialized = true;
    }

    const float coverage = std::clamp(m_Progress, 0.0f, 1.0f);

    float width  = m_BaseScale.x;
    float height = m_BaseScale.y;
    float depth  = m_BaseScale.z;

    NVector3 pos   = m_BasePos;
    NVector3 scale = m_BaseScale;

    float uOffset = 0.0f;
    float vOffset = 0.0f;
    float uScale  = 1.0f;
    float vScale  = 1.0f;

    const float halfWidth  = width * 0.5f;
    const float halfHeight = height * 0.5f;

    switch (m_Dir)
    {
    case DIRECTION::LEFT_TO_RIGHT:
        scale.x = width * coverage;
        scale.y = m_BaseScale.y;
        if (!m_Invert)
        {
            pos.x = m_BasePos.x - halfWidth + scale.x * 0.5f;
            uOffset = 0.0f;
        }
        else
        {
            pos.x = m_BasePos.x + halfWidth - scale.x * 0.5f;
            uOffset = 1.0f - coverage;
        }
        uScale = coverage;
        break;

    case DIRECTION::RIGHT_TO_LEFT:
        scale.x = width * coverage;
        scale.y = m_BaseScale.y;
        if (!m_Invert)
        {
            pos.x = m_BasePos.x + halfWidth - scale.x * 0.5f;
            uOffset = 1.0f - coverage;
        }
        else
        {
            pos.x = m_BasePos.x - halfWidth + scale.x * 0.5f;
            uOffset = 0.0f;
        }
        uScale = coverage;
        break;

    case DIRECTION::TOP_TO_BOTTOM:
        scale.x = m_BaseScale.x;
        scale.y = height * coverage;
        if (!m_Invert)
        {
            pos.y = m_BasePos.y + halfHeight - scale.y * 0.5f;
            vOffset = 0.0f;
        }
        else
        {
            pos.y = m_BasePos.y - halfHeight + scale.y * 0.5f;
            vOffset = 1.0f - coverage;
        }
        vScale = coverage;
        break;

    case DIRECTION::BOTTOM_TO_TOP:
        scale.x = m_BaseScale.x;
        scale.y = height * coverage;
        if (!m_Invert)
        {
            pos.y = m_BasePos.y - halfHeight + scale.y * 0.5f;
            vOffset = 1.0f - coverage;
        }
        else
        {
            pos.y = m_BasePos.y + halfHeight - scale.y * 0.5f;
            vOffset = 0.0f;
        }
        vScale = coverage;
        break;
    }

    SetScale(scale.x, scale.y, depth);
    SetPos(pos.x, pos.y, pos.z);

    m_UOffset = uOffset;
    m_VOffset = vOffset;
    m_UScale  = (m_Dir == DIRECTION::LEFT_TO_RIGHT || m_Dir == DIRECTION::RIGHT_TO_LEFT) ? coverage : 1.0f;
    m_VScale  = (m_Dir == DIRECTION::TOP_TO_BOTTOM || m_Dir == DIRECTION::BOTTOM_TO_TOP) ? coverage : 1.0f;
}

void Wipe::GetViewportSize(ID3D11DeviceContext* ctx, UINT& w, UINT& h)
{
    D3D11_VIEWPORT vp{};
    UINT count = 1;
    ctx->RSGetViewports(&count, &vp);
    w = static_cast<UINT>(vp.Width);
    h = static_cast<UINT>(vp.Height);
}
