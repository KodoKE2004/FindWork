#include "TransScene.h"
#include "../main.h"
#include "../Game.h"
#include <memory>

TransScene::TransScene(bool isPush, bool isPop)
{
	m_IsSceneStackPush = isPush;
    m_IsSceneStackPop  = isPop;
}

void TransScene::Initialize()
{
    auto& instance = GAME_INSTANCE;

	m_Timer = 0.0f;
	m_Alpha = 0.0f;
	m_isChange = false;
	m_Step = STEP::DOING;
	
	//================================
	//	  TransitionTextureの初期化
	//================================
	switch (m_TransMode)
	{
	case TRANS_MODE::FADE:
	{
		m_AlphaValue = 1.0f / m_Duration;
        m_TransitionTexture = std::make_shared<Fade>(instance.GetCamera());
		m_TransitionTexture->Initialize();
		m_TransitionTexture->SetPos(0.0f, 0.0f, -2.0f);
	}
	break;
	case TRANS_MODE::WIPE:
	{
        m_AlphaValue = 1.0f / m_Duration;
		m_TransitionTexture = std::make_shared<Wipe>(instance.GetCamera());
		m_TransitionTexture->Initialize();
		// 次のシーンの１フレーム目を作成と取得
		m_SceneNext->Initialize();
		DrawNextScene();
		m_TransitionTexture->SetTextureSRV(m_NextSceneSRV.Get());
		m_TransitionTexture->SetPos(0.0f, 0.0f, -2.0f);
	}
	break;
	}
    
	m_TransitionTexture->SetTimerInfo(m_Timer,m_Duration);
	instance.SetTransitionTexture(m_TransitionTexture);
}

void TransScene::Update(float tick)
{
    auto& instance = GAME_INSTANCE;
	//===========================
	//		遷移前   IN処理 
	//===========================
	if (m_isChange == false)
	{
		float timer = 0.0f;

		m_TransitionTexture->Update();

		// タイマー処理の更新
		SetTimer(m_TransitionTexture->GetTimer());
		// 次シーンに移れるか
		if (isOverClock())
		{

		}
	}
	//===========================
	//		遷移後	 OUT処理
	//===========================
	else
	{
		switch (m_TransMode)
		{
		case TRANS_MODE::FADE:
		{
			
		}
		break;
		case TRANS_MODE::WIPE:
		{
			
		}
		break;
		}
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

void TransScene::OldToNextScene()
{
	
}

// フェードイン
//void TransScene::FADE_IN()
//{
//	m_Alpha -= m_AlphaValue * Application::GetDeltaTime();
//
//	if (isOverClock())
//	{
//		m_Alpha = 0.0f;
//		m_Step = STEP::FINISH;
//	}
//
//	m_TransitionTexture->SetColor(0.0f, 0.0f, 0.0f, m_Alpha);
//	
//}

// フェードアウト
//void TransScene::FADE_OUT()
//{
//	m_Alpha += m_AlphaValue * Application::GetDeltaTime();
//
//	if (isOverClock())
//	{
//		m_Alpha = 1.0f;
//		m_isChange = true;
//	}
//
//	m_TransitionTexture->SetColor(0.0f, 0.0f, 0.0f, m_Alpha);
//}

void TransScene::WIPE_IN()
{
}

void TransScene::WIPE_OUT()
{
}
