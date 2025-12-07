#include "Zoom.h"
#include "Game.h"

Zoom::Zoom(Camera* cam) : TransitionBase(cam)
{
    
}

void Zoom::Initialize()
{   
    auto& instance = Game::GetInstance();
    TextureManager* textureMgr = instance;
    m_Texture = textureMgr->GetTexture("Black.png");

    std::vector<VERTEX_3D> vertices(4);
    vertices[0].position = NVector3(-0.5f,  0.5f, 0.0f);
    vertices[1].position = NVector3( 0.5f,  0.5f, 0.0f);
    vertices[2].position = NVector3(-0.5f, -0.5f, 0.0f);
    vertices[3].position = NVector3( 0.5f, -0.5f, 0.0f);

    for (auto& v : vertices) v.color = Color(1, 1, 1, 1);

    vertices[0].uv = Vector2(0, 0);
    vertices[1].uv = Vector2(1, 0);
    vertices[2].uv = Vector2(0, 1);
    vertices[3].uv = Vector2(1, 1);

    m_VertexBuffer.Create(vertices);
    std::vector<unsigned int> indices{ 0,1,2,3 };
    m_IndexBuffer.Create(indices);

    SetShader("VS_Alpha", "PS_CenterFade");

    m_Materiale = std::make_unique<Material>();
    MATERIAL mtrl{};
    mtrl.Diffuse = Color(1, 1, 1, 1);
    mtrl.Shiness = 1;
    mtrl.TextureEnable = true;
    m_Materiale->Create(mtrl);

    m_CenterFade = std::make_unique<CenterFade>();
    FADE_BUFFER fadeBuffer{};
    fadeBuffer.centerUV = XMFLOAT2(0.5f,0.5f);
    fadeBuffer.fadeProgress = 0.0f;
    fadeBuffer.maxInnerRadius = 0.5f;
    fadeBuffer.edgeWidth = 0.2f;
    m_CenterFade->Create(fadeBuffer);
    
    SetPos(0.0f,0.0f,0.0f);

}

void Zoom::Update(float tick)
{
    
}

void Zoom::Draw()
{
    Renderer::SetDepthEnable(false);
    Renderer::SetBlendState(BS_ALPHABLEND);

    Matrix r = Matrix::CreateFromYawPitchRoll(m_Rotation.x, m_Rotation.y, m_Rotation.z);
    Matrix t = Matrix::CreateTranslation(m_Position.x, m_Position.y, m_Position.z);
    Matrix s = Matrix::CreateScale(m_Scale.x, m_Scale.y, m_Scale.z);
    Matrix world = s * r * t;
    Renderer::SetWorldMatrix(&world);

    ID3D11DeviceContext* dc = Renderer::GetDeviceContext();
    dc->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

    SetGPU();
    m_VertexBuffer.SetGPU();
    m_IndexBuffer.SetGPU();
    m_Texture->SetGPU();

    m_Materiale->SetDiffuse(DirectX::XMFLOAT4(m_Color.x, m_Color.y, m_Color.z, m_Color.w));

    m_Materiale->Update();
    m_CenterFade->Update();

    float u = (m_NumU - 1.0f) / m_SplitX;
    float v = (m_NumV - 1.0f) / m_SplitY;
    float uw = 1.0f / m_SplitX;
    float vh = 1.0f / m_SplitY;
    Renderer::SetUV(u, v, uw, vh);
    Camera::ScopedMode scoped(m_Camera, CAMERA_2D);
    dc->DrawIndexed(4, 0, 0);
    Renderer::SetDepthEnable(true);
}

void Zoom::Finalize()
{
}

void Zoom::ZOOM_IN(float tick)
{
}

void Zoom::ZOOM_OUT(float tick)
{
}

void Zoom::SetTransMode(TRANS_MODE transMode)
{
    m_TransMode = transMode;
}
