#include "Wipe.h"
#include "Game.h"
#include "main.h"
#include "Renderer.h"
#include "Application.h"
#include "Calculator.h"
#include <algorithm>
#include "Debug.hpp"

using namespace Math::Easing;

Wipe::Wipe(Camera& cam) : TransitionBase(cam)
{
}

void Wipe::Initialize()
{
    TextureManager* textureMgr = Game::GetInstance();
    m_Texture = textureMgr->GetTexture("Black.png");

    SetScale(static_cast<float>(Renderer::GetScreenWidth()),
             static_cast<float>(Renderer::GetScreenHeight()),
             1.0f);

    SetColor(0.0f, 0.0f, 0.0f, 1.0f);

    m_Phase = TRANS_PHASE::TRANS_IN;
    m_Elapsed = 0.0f;

    if (m_Duration <= 0.0f) m_Duration = 1.0f;

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

    SetShader("VS_Alpha", "PS_Alpha");

    m_Material = std::make_unique<Material>();
    MATERIAL mtrl{};
    mtrl.Diffuse = Color(1, 1, 1, 1);
    mtrl.Shiness = 1;
    mtrl.TextureEnable = true;
    m_Material->Create(mtrl);
}


void Wipe::Update(float tick)
{
    switch (m_Phase)
    {
    case TRANS_PHASE::TRANS_OUT: WIPE_OUT(tick); break;
    case TRANS_PHASE::TRANS_IN : WIPE_IN (tick); break;
    default: break;
    }
}

void Wipe::Draw()
{
    // State破壊の影響を受けないよう、パイプラインを先頭で再設定する
    SetPipeline();

    static uint64_t s_LastLogFrame = 0;
    const auto frame = Game::GetDrawFrameCounter();
    if (frame != s_LastLogFrame) {
        s_LastLogFrame = frame;
    }

    if (m_DebugSolidDraw) {
        DrawDebugFullscreenSolid();
        return;
    }

    if (auto* srv = GetTextureSRV())
    {
        if (m_Phase == TRANS_PHASE::FINISH)
        {
            Renderer::SetDepthEnable(false);
            Renderer::SetBlendState(BS_ALPHABLEND);
            Renderer::BlitSRVToBackbuffer(srv);
            Renderer::SetDepthEnable(true);
            return;
        }
    }

    Renderer::SetDepthEnable(false);
    Renderer::SetBlendState(BS_ALPHABLEND);

    Matrix r = Matrix::CreateFromYawPitchRoll(m_Rotation.x, m_Rotation.y, m_Rotation.z);
    Matrix t = Matrix::CreateTranslation(m_Position.x, m_Position.y, m_Position.z);
    Matrix s = Matrix::CreateScale(m_Scale.x, m_Scale.y, m_Scale.z);
    Matrix world = s * r * t;
    Renderer::SetWorldMatrix(&world);

    ID3D11DeviceContext* dc = Renderer::GetDeviceContext();
    if (m_Texture)
    {
        m_Texture->SetGPU();
    }

    if (m_Material)
    {
        m_Material->SetDiffuse(DirectX::XMFLOAT4(m_Color.x, m_Color.y, m_Color.z, m_Color.w));
        m_Material->Update();
        m_Material->SetGPU();
    }

    float u = (m_NumU - 1.0f) / m_SplitX;
    float v = (m_NumV - 1.0f) / m_SplitY;
    float uw = 1.0f / m_SplitX;
    float vh = 1.0f / m_SplitY;
    Renderer::SetUV(u, v, uw, vh);

    Camera::ScopedMode scoped(m_Camera, CAMERA_2D);
    dc->DrawIndexed(4, 0, 0);
    Renderer::SetDepthEnable(true);
}
void Wipe::Finalize() {}


void Wipe::ApplyWipeAmount(float amount)
{
    // 進行状態のケア
    amount = std::clamp(amount, 0.0f, 1.0f);

    // 初期状態は全画面表示
    const float depth = GetPos().z;
    NVector3 start,end;
    if (m_Phase == TRANS_PHASE::TRANS_IN)
    {
        start = NVector3(m_StartPos.x, m_StartPos.y, depth);
        end   = NVector3(m_CheckPoint.x, m_CheckPoint.y, depth);
    }
    else if (m_Phase == TRANS_PHASE::TRANS_OUT)
    {
        start = NVector3(m_CheckPoint.x, m_CheckPoint.y, depth);
        end   = NVector3(m_EndPos.x  , m_EndPos.y, depth);
    }
    NVector3 pos;

    pos.x = start.x + (end.x - start.x) * amount;
    pos.y = start.y + (end.y - start.y) * amount;
    pos.z = depth;

    SetPos(pos);

}

float Wipe::CalculateWipeEasing(const SceneTransitionParam& param, float t)
{
    if (param.easing != EASING_TYPE::NONE) {
        return Math::Easing::EvaluateEasing(param.easing, t);
    }
    else {
        return t;
    }
}


void Wipe::WIPE_IN(float tick)
{
    if (m_Phase != TRANS_PHASE::TRANS_IN) return;

    m_Elapsed += tick;

    const auto& param = m_transParam;
    const float duration = GetDurationForPhase() * 0.5f;
    const float t = std::clamp(m_Elapsed / max(duration , 0.0001f), 0.0f, 1.0f);
    float eased = CalculateWipeEasing(param, t);

    ApplyWipeAmount(eased);

    if (t >= 1.0f)
    {
        ApplyPhaseSetting(TRANS_PHASE::TRANS_OUT);
        SetPos(0.0f,0.0f,0.0f);
        m_isChange = true;
        m_Elapsed = 0.0f;
    }
}

void Wipe::WIPE_OUT(float tick)
{
    if (m_Phase != TRANS_PHASE::TRANS_OUT) return;

    m_Elapsed += tick;

    const auto& param = m_transParam;
    const float duration = GetDurationForPhase();
    const float t = std::clamp(m_Elapsed / max(duration * 0.5f, 0.0001f), 0.0f, 1.0f);
    float eased = CalculateWipeEasing(param, t);

    ApplyWipeAmount(eased);

    if (t >= 1.0f)
    {
        m_Phase = TRANS_PHASE::FINISH;
        m_Elapsed = 0.0f;
    }
}

void Wipe::SetTransMode(TRANS_MODE transMode)
{
    m_TransMode = transMode;
    switch (m_TransMode)
    {
    default: 
    case TRANS_MODE::WIPE_LEFT_TO_RIGHT: SetLeftToRight(); break;
    case TRANS_MODE::WIPE_RIGHT_TO_LEFT: SetRightToLeft(); break;
    case TRANS_MODE::WIPE_TOP_TO_BOTTOM: SetTopToBottom(); break;
    case TRANS_MODE::WIPE_BOTTOM_TO_TOP: SetBottomToTop(); break;
    }
    
    CheckPointSetting();
    SetPos(m_StartPos);

}

void Wipe::CheckPointSetting()
{
    // 定数設定
    const float widthHalf  = static_cast<float>(Application::GetWidth())  * 0.5f;
    const float heightHalf = static_cast<float>(Application::GetHeight()) * 0.5f;

    // 中間地点は原点
    m_CheckPoint = NVector3(0.0f, 0.0f ,0.0f);

    float massX,massY;
    massX = 1.0f;
    massY = 1.0f;
    switch (m_TransMode)
    {
    case TRANS_MODE::WIPE_LEFT_TO_RIGHT: massX = - 1.0f; massY =   0.0f; break;
    case TRANS_MODE::WIPE_RIGHT_TO_LEFT: massX =   1.0f; massY =   0.0f; break;
    case TRANS_MODE::WIPE_TOP_TO_BOTTOM: massX =   0.0f; massY =   1.0f; break;
    case TRANS_MODE::WIPE_BOTTOM_TO_TOP: massX =   0.0f; massY = - 1.0f; break;
    }

    NVector3 startPos;
    startPos.x = (widthHalf  * massX) + (GetScale().x * 0.5f * massX);
    startPos.y = (heightHalf * massY) + (GetScale().y * 0.5f * massY);
    startPos.z = GetPos().z;

    NVector3 endPos;
    endPos.x = - startPos.x ;
    endPos.y = - startPos.y ;
    endPos.z =   startPos.z ;
    
    m_StartPos = NVector3(startPos);
    m_EndPos   = NVector3(endPos)  ;

}
