// SceneFile/Transition/Wipe.cpp
#include "Wipe.h"
#include "Renderer.h"
#include "../../Application.h"
        // Cvp̉摜ǉ
        auto textureMgr = GAME_MANAGER_TEXTURE;
        m_Texture = textureMgr->GetTexture("Black.png");
        SetScale(SCREEN_WIDTH, SCREEN_HEIGHT, 1.0f);
        SetColor(0.0f, 0.0f, 0.0f, 0.0f);
        m_Phase = WIPE_PHASE::OUT;
        m_Rate  = 0.0f;
        m_Timer = 0.0f;
        if (m_Duration <= 0.0f)
        {
                m_Duration = 1.0f;
        }
        SetTimerInfo(0.0f, m_Duration, m_Duration);

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
        switch (m_Phase)
        {
        case WIPE_PHASE::OUT:
                WIPE_OUT();
                break;
        case WIPE_PHASE::IN:
                WIPE_IN();
                break;
        default:
                break;
        }
        // SRVZbgĂȂÎ~𒼐ڃubgiJڏIɎgpj
        if (auto* srv = GetTextureSRV())
        {
                if (m_Phase == WIPE_PHASE::FINISH)
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

        Renderer::SetDepthEnable(true);  // `Ɍ֖߂
}

void Wipe::Finalize()
{
}
void Wipe::ApplyWipeAmount(float amount)
{
        amount = std::clamp(amount, 0.0f, 1.0f);
        m_Rate  = amount;

        float width  = SCREEN_WIDTH;
        float height = SCREEN_HEIGHT;
        float posX   = 0.0f;
        float posY   = 0.0f;
        const float depth = GetPos().z;

        switch (m_Mode)
        {
        case WIPE_MODE::LEFT_TO_RIGHT:
                width = SCREEN_WIDTH * amount;
                posX  = (width - SCREEN_WIDTH) * 0.5f;
                break;
        case WIPE_MODE::RIGHT_TO_LEFT:
                width = SCREEN_WIDTH * amount;
                posX  = (SCREEN_WIDTH - width) * 0.5f;
                break;
        case WIPE_MODE::TOP_TO_BOTTOM:
                height = SCREEN_HEIGHT * amount;
                posY   = (height - SCREEN_HEIGHT) * 0.5f;
                break;
        case WIPE_MODE::BOTTOM_TO_TOP:
                height = SCREEN_HEIGHT * amount;
                posY   = (SCREEN_HEIGHT - height) * 0.5f;
                break;
        default:
                break;
        }

        // Eɒ[ɏȂȂ悤ɉ݂
        width  = std::max(width, 0.0f);
        height = std::max(height, 0.0f);

        SetScale(width, height, 1.0f);
        SetPos(posX, posY, depth);
        SetColor(0.0f, 0.0f, 0.0f, amount > 0.0f ? 1.0f : 0.0f);
}

void Wipe::WIPE_IN()
{
        if (m_Phase != WIPE_PHASE::IN)
        {
                return;
        }

        float delta = Application::GetDeltaTime();
        CountTimer(delta);
        float progress = 1.0f - std::clamp(m_Timer / std::max(m_Duration, 0.0001f), 0.0f, 1.0f);
        ApplyWipeAmount(progress);

        if (progress <= 0.0f)
        {
                m_Phase = WIPE_PHASE::FINISH;
                SetScale(SCREEN_WIDTH, SCREEN_HEIGHT, 1.0f);
                SetPos(0.0f, 0.0f, GetPos().z);
                SetColor(0.0f, 0.0f, 0.0f, 0.0f);
        }
void Wipe::WIPE_OUT()
        if (m_Phase != WIPE_PHASE::OUT)
        {
                return;
        }

        float delta = Application::GetDeltaTime();
        CountTimer(delta);
        float progress = std::clamp(m_Timer / std::max(m_Duration, 0.0001f), 0.0f, 1.0f);
        ApplyWipeAmount(progress);

        if (progress >= 1.0f)
        {
                m_Phase = WIPE_PHASE::IN;
                ResetTimer();
        }

	m_Materiale->SetGPU();

	// UVの設定を指定
	float u = m_NumU - 1;
	float v = m_NumV - 1;
	float uw = 1 / m_SplitX;
	float vh = 1 / m_SplitY;

	Renderer::SetUV(u, v, uw, vh);
	Camera::ScopedMode scepedMode(m_Camera, CAMERA_2D);

	devicecontext->DrawIndexed(
		4, // 描画するインデックス数（四角形なんで４）
		0, // 最初のインデックスバッファの位置
		0);

	Renderer::SetDepthEnable(true);  // 描画後に元へ戻す
}

void Wipe::Finalize()
{
}
