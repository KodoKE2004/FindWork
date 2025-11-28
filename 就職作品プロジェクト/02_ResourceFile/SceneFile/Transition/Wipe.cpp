#include "Game.h"
#include "main.h"
#include "Renderer.h"
#include "Application.h"
#include "Collider.h"
#include <algorithm>

Wipe::Wipe(Camera* cam) : TransitionBase(cam)
{
}

void Wipe::Initialize()
{
    TextureManager* textureMgr = Game::GetInstance();
    m_Texture = textureMgr->GetTexture("Black.png");

    SetScale(SCREEN_WIDTH, SCREEN_HEIGHT, 1.0f);
    SetColor(0.0f, 0.0f, 0.0f, 0.0f);

    m_Phase = PHASE::TRANS_OUT;
    m_Rate = 0.0f;
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

    m_Materiale = std::make_unique<Material>();
    MATERIAL mtrl{};
    mtrl.Diffuse = Color(1, 1, 1, 1);
    mtrl.Shiness = 1;
    mtrl.TextureEnable = true;
    m_Materiale->Create(mtrl);
}


void Wipe::Update(float tick)
{
    switch (m_Phase)
    {
    case PHASE::TRANS_OUT: WIPE_OUT(tick); break;
    case PHASE::TRANS_IN : WIPE_IN (tick); break;
    default: break;
    }
}


void Wipe::Draw()
{

    if (auto* srv = GetTextureSRV())
    {
        if (m_Phase == PHASE::FINISH)
        {
            Renderer::SetDepthEnable(false);
            Renderer::SetBlendState(BS_ALPHABLEND);
            Renderer::BlitSRVToBackbuffer(srv, 1.0f);
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
    dc->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

    SetGPU();
    m_VertexBuffer.SetGPU();
    m_IndexBuffer.SetGPU();
    if (m_Texture)
    {
        m_Texture->SetGPU();
    }

    if (m_Materiale)
    {
        m_Materiale->SetDiffuse(DirectX::XMFLOAT4(m_Color.x, m_Color.y, m_Color.z, m_Color.w));
        m_Materiale->Update();
        m_Materiale->SetGPU();
    }

    float u = m_NumU - 1;
    float v = m_NumV - 1;
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
    m_Rate = amount;

    // 初期状態は全画面表示
    float width = SCREEN_WIDTH;
    float height = SCREEN_HEIGHT;
    float posX = 0.0f;
    float posY = 0.0f;
    const float depth = GetPos().z;

    // 方向ごとに処理
    switch (m_Mode)
    {
    case WIPE_MODE::LEFT_TO_RIGHT:
        width = SCREEN_WIDTH * amount;
        posX = (width - SCREEN_WIDTH) * 0.5f;
        break;
    case WIPE_MODE::RIGHT_TO_LEFT:
        width = SCREEN_WIDTH * amount;
        posX = (SCREEN_WIDTH - width) * 0.5f;
        break;
    case WIPE_MODE::TOP_TO_BOTTOM:
        height = SCREEN_HEIGHT * amount;
        posY = (height - SCREEN_HEIGHT) * 0.5f;
        break;
    case WIPE_MODE::BOTTOM_TO_TOP:
        height = SCREEN_HEIGHT * amount;
        posY = (SCREEN_HEIGHT - height) * 0.5f;
        break;
    default:
        break;
    }

    // 負の値にならないようにケア
    width  = max(width, 0.0f);
    height = max(height, 0.0f);

    SetScale(width, height, 1.0f);
    SetPos(posX, posY, depth);
    SetColor(0.0f, 0.0f, 0.0f, amount > 0.0f ? 1.0f : 0.0f);
}


void Wipe::WIPE_IN(float tick)
{
    if (m_Phase != PHASE::TRANS_IN) return;

    m_Elapsed += tick;
    const float t = std::clamp(m_Elapsed / max(m_Duration, 0.0001f), 0.0f, 1.0f);
    const float eased = Math::Easing::EaseInOutBack(t);

    ApplyWipeAmount(1.0f - eased);

    if (t >= 1.0f)
    {
        m_Phase = PHASE::FINISH;
        m_Elapsed = 0.0f;
        SetScale(SCREEN_WIDTH, SCREEN_HEIGHT, 1.0f);
        SetPos(0.0f, 0.0f, GetPos().z);
        SetColor(0.0f, 0.0f, 0.0f, 0.0f);
    }
}

void Wipe::WIPE_OUT(float tick)
{
    if (m_Phase != PHASE::TRANS_OUT) return;

    m_Elapsed += tick;
    const float t = std::clamp(m_Elapsed / (m_Duration, 0.0001f), 0.0f, 1.0f);
    const float eased = Math::Easing::EaseInOutSine(t);

    ApplyWipeAmount(eased);

    if (t >= 1.0f)
    {
        m_Phase = PHASE::TRANS_IN;
        m_Elapsed = 0.0f;
    }
}

void Wipe::SetTransMode(TRANS_MODE transMode)
{
    m_TransMode = transMode;
    switch (m_TransMode)
    {
    default:
    case TRANS_MODE::WIPE_LEFT_TO_RIGHT:
        SetLeftToRight();
    break;
    case TRANS_MODE::WIPE_RIGHT_TO_LEFT:
        SetRightToLeft();
    break;
    case TRANS_MODE::WIPE_TOP_TO_BOTTOM:
        SetTopToBottom();
    break;
    case TRANS_MODE::WIPE_BOTTOM_TO_TOP:
        SetBottomToTop();
    break;
    }
}
