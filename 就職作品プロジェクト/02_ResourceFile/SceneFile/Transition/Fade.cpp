#include "Fade.h"
#include "../../Game.h"
#include "../../main.h"
#include "../../Application.h"
#include <vector>
#include <algorithm>


Fade::Fade(Camera* cam) : TransitionBase(cam)
{
    m_Phase = FADE_PHASE::TRANS_OUT;
}


void Fade::Initialize()
{
    // テクスチャ（真っ黒）
    auto textureMgr = GAME_MANAGER_TEXTURE;
    m_Texture = textureMgr->GetTexture("Black.png");

    SetScale(SCREEN_WIDTH, SCREEN_HEIGHT, 1.0f);
    m_Alpha = 0.0f;
    m_Timer = 0.0f;

    if (m_Duration <= 0.0f) {
        m_Duration = 1.0f;
    }

    SetTimerInfo(0.0f, m_Duration, m_Duration);

    // 秒あたりのアルファ変化量
    m_AlphaValue = 1.0f / max(m_Duration, 0.0001f);
    ApplyAlpha();

    // フルスクリーン矩形
    std::vector<VERTEX_3D> vertices(4);
    vertices[0].position = NVector3(-0.5f, 0.5f, 0.0f);
    vertices[1].position = NVector3(0.5f, 0.5f, 0.0f);
    vertices[2].position = NVector3(-0.5f, -0.5f, 0.0f);
    vertices[3].position = NVector3(0.5f, -0.5f, 0.0f);

    for (auto& v : vertices) v.color = Color(1, 1, 1, 1);

    vertices[0].uv = Vector2(0.0f, 0.0f);
    vertices[1].uv = Vector2(1.0f, 0.0f);
    vertices[2].uv = Vector2(0.0f, 1.0f);
    vertices[3].uv = Vector2(1.0f, 1.0f);

    m_VertexBuffer.Create(vertices);

    std::vector<unsigned int> indices{ 0,1,2,3 };
    m_IndexBuffer.Create(indices);

    SetShader("VS_Alpha", "PS_Alpha");

    // マテリアル
    m_Materiale = std::make_unique<Material>();
    MATERIAL mtrl{};
    mtrl.Diffuse = Color(1, 1, 1, 1);
    mtrl.Shiness = 1;
    mtrl.TextureEnable = true;
    m_Materiale->Create(mtrl);
}

void Fade::Update()
{
    switch (m_Phase)
    {
    case FADE_PHASE::TRANS_OUT: FADE_OUT(); break;
    case FADE_PHASE::TRANS_IN: FADE_IN(); break;
    default: break;
    }
}
void Fade::Draw()
{
    Renderer::SetDepthEnable(false);
    Renderer::SetBlendState(BS_ALPHABLEND);


    // SRT
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
    m_Materiale->SetGPU();

    // UV 設定
    float u = m_NumU - 1;
    float v = m_NumV - 1;
    float uw = 1.0f / m_SplitX;
    float vh = 1.0f / m_SplitY;
    Renderer::SetUV(u, v, uw, vh);
    Camera::ScopedMode scoped(m_Camera, CAMERA_2D);
    dc->DrawIndexed(4, 0, 0);
    Renderer::SetDepthEnable(true);
}

void Fade::Finalize() {}

void Fade::ApplyAlpha()
{
    m_Alpha = std::clamp(m_Alpha, 0.0f, 1.0f);
    SetColor(0.0f, 0.0f, 0.0f, m_Alpha);
}

void Fade::FADE_IN()
{
    if (m_Phase != FADE_PHASE::TRANS_IN) return;

    float delta = Application::GetDeltaTime();
    CountTimer(delta);

    m_Alpha -= m_AlphaValue * delta;
    ApplyAlpha();

    if (m_Alpha <= 0.0f)
    {
        m_Alpha = 0.0f;
        ApplyAlpha();
        m_Phase = FADE_PHASE::FINISH;
    }
}


void Fade::FADE_OUT()
{
    if (m_Phase != FADE_PHASE::TRANS_OUT) return;

    float delta = Application::GetDeltaTime();
    CountTimer(delta);

    m_Alpha += m_AlphaValue * delta;
    ApplyAlpha();

    if (m_Alpha >= 1.0f)
    {
        m_Alpha = 1.0f;
        ApplyAlpha();
        m_Phase = FADE_PHASE::TRANS_IN;
        ResetTimer();
    }
}