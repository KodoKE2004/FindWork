#include "Fade.h"
#include "Game.h"
#include "main.h"
#include "Application.h"
#include "Collider.h"
#include "Renderer.h"
#include <vector>
#include <algorithm>

Fade::Fade(Camera& cam) : TransitionBase(cam)
{
}

void Fade::Initialize()
{
    // 初期カラーを透明に設定
    m_Alpha = 0.0f;
    m_Elapsed = 0.0f;

    // Durationのケア
    if (m_Duration <= 0.0f) {
        m_Duration = 1.0f;
    }

    // アルファ値の計算と適用
    ApplyAlpha();

    // フェードアウトから開始
    ApplyPhaseSetting(TRANS_PHASE::TRANS_OUT);

    // テクスチャの取得
    TextureManager* textureMgr = Game::GetInstance();
    m_Texture = textureMgr->GetTexture("Black.png");

    // フルスクリーンに設定
    SetScale(static_cast<float>(Renderer::GetScreenWidth()),
             static_cast<float>(Renderer::GetScreenHeight()),
             1.0f);

#ifdef _DEBUG
    SetPos(ConvertToDebugScreenPosition(GetPos()));
#endif

    std::vector<VERTEX_3D> vertices(4);
    vertices[0].position = NVector3(-0.5f,  0.5f, 0.0f);
    vertices[1].position = NVector3( 0.5f,  0.5f, 0.0f);
    vertices[2].position = NVector3(-0.5f, -0.5f, 0.0f);
    vertices[3].position = NVector3( 0.5f, -0.5f, 0.0f);

    for (auto& v : vertices) v.color = Color(1, 1, 1, 1);

    vertices[0].uv = Vector2(0.0f, 0.0f);
    vertices[1].uv = Vector2(1.0f, 0.0f);
    vertices[2].uv = Vector2(0.0f, 1.0f);
    vertices[3].uv = Vector2(1.0f, 1.0f);

    m_VertexBuffer.Create(vertices);

    std::vector<unsigned int> indices{ 0,1,2,3 };
    m_IndexBuffer.Create(indices);

    SetShader("VS_Alpha", "PS_Alpha");

    m_Materiale = std::make_unique<Material>();
    MATERIAL mtrl{};
    mtrl.Diffuse = Color(1, 1, 1, 1);
    mtrl.Shiness = 1;
    mtrl.TextureEnable = true;
    m_Materiale->Create(mtrl);
}

void Fade::Update(float tick)
{
    switch (m_Phase)
    {
    case TRANS_PHASE::TRANS_OUT: FADE_OUT(tick); break;
    case TRANS_PHASE::TRANS_IN:  FADE_IN(tick); break;
    default: break;
    }
}

void Fade::Draw()
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
    m_Materiale->SetGPU();

    float u = (m_NumU - 1.0f) / m_SplitX;
    float v = (m_NumV - 1.0f) / m_SplitY;
    float uw = 1.0f / m_SplitX;
    float vh = 1.0f / m_SplitY;
    Renderer::SetUV(u, v, uw, vh);
    Camera::ScopedMode scoped(m_Camera, CAMERA_2D);
    dc->DrawIndexed(4, 0, 0);
    Renderer::SetDepthEnable(true);
}

void Fade::Finalize() 
{
}

void Fade::ApplyAlpha()
{
    m_Alpha = std::clamp(m_Alpha, 0.0f, 1.0f);
    SetColor(1.0f, 1.0f, 1.0f, m_Alpha);
}

void Fade::SetTransMode(TRANS_MODE transMode)
{
    m_TransMode = transMode;
}

void Fade::FADE_IN(float tick)
{
    if (m_Phase != TRANS_PHASE::TRANS_IN) return;

    m_Elapsed += tick;
    const auto& param = m_transParam;
    const float duration = param.duration;
    const float t = std::clamp(m_Elapsed /max(duration, 0.0001f), 0.0f, 1.0f);
    const float eased = EvaluateEasing(param, t);

    m_Alpha = 1.0f - eased;

    if (t >= 1.0f)
    {
        m_Alpha = 0.0f;
        m_Phase = TRANS_PHASE::FINISH;
    }

    ApplyAlpha();
}

void Fade::FADE_OUT(float tick)
{
    if (m_Phase != TRANS_PHASE::TRANS_OUT) return;

    m_Elapsed += tick;
    const auto& param = m_transParam;
    const float duration = param.duration;
    const float t = std::clamp(m_Elapsed / max(duration, 0.0001f), 0.0f, 1.0f);
    const float eased = EvaluateEasing(param, t);

    m_Alpha = eased;
    if (t >= 1.0f)
    {
        m_Alpha = 1.0f;
        ApplyPhaseSetting(TRANS_PHASE::TRANS_IN);
        m_isChange = true;
        m_Elapsed = 0.0f;
    }
    ApplyAlpha();
}