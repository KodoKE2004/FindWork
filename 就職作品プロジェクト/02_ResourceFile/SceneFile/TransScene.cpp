#include "TransScene.h"
#include "../main.h"
#include "../Game.h"

void TransScene::Initialize()
{
    auto& instance = GAME_INSTANCE;

	m_Timer = 0.0f;
	m_Alpha = 0.0f;
	m_isChange = false;
	m_Step = STEP::DOING;

	switch (m_TransMode)
	{
	case TRANS_MODE::FADE:
	{
		m_AlphaValue = 1.0f / m_Duration;
        auto fade = std::make_shared<Fade>(instance.GetCamera());
        fade->Initialize();
        fade->SetPos(0.0f, 0.0f, -2.0f);
		m_TransitionTexture = fade;
        instance.SetTransitionTexture(m_TransitionTexture);
	}
	break;
	case TRANS_MODE::WIPE:
	{
        m_AlphaValue = 1.0f / m_Duration;
		auto wipe = std::make_shared<Wipe>(instance.GetCamera());
		wipe->Initialize();
        wipe->SetPos(0.0f, 0.0f, -2.0f);
        m_TransitionTexture = wipe;
        instance.SetTransitionTexture(m_TransitionTexture);
	}
	break;
	}
}

void TransScene::Update(float tick)
{
    auto& instance = GAME_INSTANCE;

	switch (m_Step)
	{
	case STEP::START:
	{
		Initialize();
	}
	break;
	case STEP::DOING:
	{
		m_Timer += tick;

		// OUTの処理
		if (!m_isChange) 
		{
			switch (m_TransMode) 
			{
			case TRANS_MODE::FADE:
			{
				FADE_OUT();		
				if (m_isChange) {
					m_SceneOld ->Finalize()  ;
					m_SceneNext->Initialize();
					DrawNextScene();			 // 次シーンをオフスクリーンに描画
				}
			}
			break;
			case TRANS_MODE::WIPE:
			{
				WIPE_OUT();
				if (m_isChange) {
					m_SceneOld->Finalize();
					m_SceneNext->Initialize();
                }
			}

			}
		}

		// INの処理
		else
		{
			switch (m_TransMode)
			{
			case TRANS_MODE::FADE:	 FADE_IN();		break;

			}
		}


	}
	break;
	case STEP::FINISH:
	{
		instance.SetSceneCurrent(m_SceneNext);
		Finalize();
	}
	break;

	}
	
}

void TransScene::Finalize()
{
    auto& instance = Game::GetInstance();
	m_Step = STEP::OFF;
	for (auto obj : m_MySceneObjects)
	{
		Game::GetInstance().DeleteObject(obj);
	}
	m_MySceneObjects.clear();

	m_Overlay = nullptr;
	m_TransitionTexture = nullptr ;
	instance.SetTransitionTexture(nullptr);
	m_NextSceneSRV.Reset();
	m_RenderTarget.reset();

#ifdef _DEBUG
	GAME_INSTANCE.m_Grid.DeInitialized();
#endif // _DEBUG

	if (this != nullptr)
	{
		delete this;
	}

}

void TransScene::DrawNextScene()
{

	// ★ 次シーンを一度だけオフスクリーンへ描画して SRV を確保
	auto* device  = Renderer::GetDevice();
	auto* context = Renderer::GetDeviceContext();
	auto  vp	  = Renderer::GetViewport();

	m_RenderTarget = std::make_unique<RenderTarget>();
	// sRGB 運用なら: m_RenderTarget->SetFormat(DXGI_FORMAT_R8G8B8A8_UNORM_SRGB);
	m_RenderTarget->Create(device, (UINT)vp.Width, (UINT)vp.Height, true);

	const float clear[4]{ 0, 0, 0, 0 };
	m_RenderTarget->Begin(context, clear);
	{
		// Update は走らせず、次シーンのオブジェクトをそのまま 1 回描く
		for (auto* obj : m_SceneNext->GetSceneObjects()) {
			if (obj) obj->Draw();
		}
	}
	RenderTarget::End(context, Renderer::GetBackBufferRTV(), vp);

	m_NextSceneSRV = m_RenderTarget->GetSRV();

	// ★ 遷移オーバーレイを現シーンに追加（最後にα合成）
	m_Overlay = Game::GetInstance().AddObject<SnapshotOverlay>();
	m_Overlay->SetSRV(m_NextSceneSRV.Get());
	m_Overlay->SetAlpha(0.0f);
	m_MySceneObjects.emplace_back(m_Overlay); // 管理用（Finalizeで掃除）

}

bool TransScene::isOverClock()
{
	if (m_Timer >= m_Duration)
	{
		m_Timer = 0.0f;
		return true;
	}

	return false;
}

// フェードイン
void TransScene::FADE_IN()
{
	m_Alpha -= m_AlphaValue * Application::GetDeltaTime();

	if (isOverClock())
	{
		m_Alpha = 0.0f;
		m_Step = STEP::FINISH;
	}

	m_TransitionTexture->SetColor(0.0f, 0.0f, 0.0f, m_Alpha);
	
}

// フェードアウト
void TransScene::FADE_OUT()
{
	m_Alpha += m_AlphaValue * Application::GetDeltaTime();

	if (isOverClock())
	{
		m_Alpha = 1.0f;
		m_isChange = true;
	}

	m_TransitionTexture->SetColor(0.0f, 0.0f, 0.0f, m_Alpha);
}

void TransScene::WIPE_IN()
{
}

void TransScene::WIPE_OUT()
{
}
