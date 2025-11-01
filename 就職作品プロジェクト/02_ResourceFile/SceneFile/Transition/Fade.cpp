#include "Fade.h"
#include "../../Game.h"
#include "../../main.h"
#include "../../Application.h"
#include <vector>
#include <algorithm>

Fade::Fade(Camera* cam) : TransitionBase(cam)
{
        m_Phase = FADE_PHASE::OUT;
}

void Fade::Initialize()
{
        // tF[hp̉摜ǉ
    auto textureMgr = GAME_MANAGER_TEXTURE;
        m_Texture = textureMgr->GetTexture("Black.png");
        SetScale(SCREEN_WIDTH, SCREEN_HEIGHT, 1.0f);
        m_Alpha = 0.0f;
        m_Timer = 0.0f;
        if (m_Duration <= 0.0f)
        {
                m_Duration = 1.0f;
        }
        SetTimerInfo(0.0f, m_Duration, m_Duration);
        m_AlphaValue = 1.0f / std::max(m_Duration, 0.0001f);
        ApplyAlpha();

        // _f[^
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

        // _obt@
        m_VertexBuffer.Create(vertices);

        // CfbNXobt@
        std::vector<unsigned int> indices;
        indices.resize(4);

        indices[0] = 0;
        indices[1] = 1;
        indices[2] = 2;
        indices[3] = 3;

        // CfbNXobt@
        m_IndexBuffer.Create(indices);

        // VF[_IuWFNg
        SetShader("VS_Alpha", "PS_Alpha");

        // }eA擾
        m_Materiale = std::make_unique<Material>();
        MATERIAL mtrl;
        mtrl.Diffuse = Color(1.0f, 1.0f, 1.0f, 1.0f);
        mtrl.Shiness = 1;
        mtrl.TextureEnable = true; // eNX`gۂ̃tO
        m_Materiale->Create(mtrl);

}
void Fade::Update()
{
        switch (m_Phase)
        {
        case FADE_PHASE::OUT:
                FADE_OUT();
                break;
        case FADE_PHASE::IN:
                FADE_IN();
                break;
        default:
                break;
        }
}
void Fade::Draw()
{
        Renderer ::SetDepthEnable(false);
        Renderer ::SetBlendState(BS_ALPHABLEND);
        Renderer::SetBlendState(BS_ALPHABLEND);

        // SRT쐬
        Matrix r = Matrix::CreateFromYawPitchRoll(m_Rotation.x, m_Rotation.y, m_Rotation.z);
        Matrix t = Matrix::CreateTranslation(m_Position.x, m_Position.y, m_Position.z);
        Matrix s = Matrix::CreateScale(m_Scale.x, m_Scale.y, m_Scale.z);

        Matrix worldmtx;
        worldmtx = s * r * t;
        Renderer::SetWorldMatrix(&worldmtx); // GPUɃZbg

        // `̏
        ID3D11DeviceContext* devicecontext;
        devicecontext = Renderer::GetDeviceContext();


        // g|W[Zbgiv~eBu^Cvj
        devicecontext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

        SetGPU();

        m_VertexBuffer.SetGPU();
        m_IndexBuffer.SetGPU();

        m_Texture->SetGPU();

        m_Materiale->SetDiffuse(DirectX::XMFLOAT4(m_Color.x, m_Color.y, m_Color.z, m_Color.w));
        m_Materiale->Update();
        m_Materiale->SetGPU();

        // UV̐ݒw
        float u = m_NumU - 1;
        float v = m_NumV - 1;
        float uw = 1 / m_SplitX;
        float vh = 1 / m_SplitY;

        Renderer::SetUV(u, v, uw, vh);
        Camera::ScopedMode scepedMode(m_Camera, CAMERA_2D);

        devicecontext->DrawIndexed(
                4, // `悷CfbNXilp`ȂłSj
                0, // ŏ̃CfbNXobt@̈ʒu
                0);

        Renderer ::SetDepthEnable(true);  // `Ɍ֖߂
}
void Fade::Finalize()
{

}

void Fade::ApplyAlpha()
{
        m_Alpha = std::clamp(m_Alpha, 0.0f, 1.0f);
        SetColor(0.0f, 0.0f, 0.0f, m_Alpha);
}

void Fade::FADE_IN()
{
        if (m_Phase != FADE_PHASE::IN)
        {
                return;
        }

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
        if (m_Phase != FADE_PHASE::OUT)
        {
                return;
        }

        float delta = Application::GetDeltaTime();
        CountTimer(delta);
        m_Alpha += m_AlphaValue * delta;
        ApplyAlpha();

        if (m_Alpha >= 1.0f)
        {
                m_Alpha = 1.0f;
                ApplyAlpha();
                m_Phase = FADE_PHASE::IN;
                ResetTimer();
        }
}

