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
	m_HasInitializedNextScene = false;
	if (m_Duration <= 0.0f) {
		m_Duration = 0.0f;
	}
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
		instance.SetTransitionTexture(m_TransitionTexture);
	}
	break;
	case TRANS_MODE::WIPE:
	{
        m_AlphaValue = 1.0f / m_Duration;
		m_TransitionTexture = std::make_shared<Wipe>(instance.GetCamera());
		m_TransitionTexture->Initialize();
		if (m_StackOp != STACK_OP::POP && m_SceneNext) {
			m_SceneNext->Initialize();
			m_HasInitializedNextScene = true;
		}
		// 次のシーンの１フレーム目を作成と取得
		if (m_SceneNext) {
			DrawNextScene();
		}

		m_TransitionTexture->SetTextureSRV(m_NextSceneSRV.Get());
		m_TransitionTexture->SetPos(0.0f, 0.0f, -2.0f);
		instance.SetTransitionTexture	 (m_TransitionTexture);
	}
	break;
	}

    m_TransitionTexture->SetPhase	 (PHASE::TRANS_IN);
	m_TransitionTexture->SetTimerInfo(0.0f, m_Duration);

}

void TransScene::Update(float tick)
{
    auto& instance = GAME_INSTANCE;

	//===========================
	//			演出処理 
	//===========================
	if (m_TransitionTexture == nullptr) {
		return;
	}

	m_TransitionTexture->Update();

	// 次シーンに移れるか
	const auto phase = m_TransitionTexture->GetPhase();
	if (!m_isChange && phase == PHASE::TRANS_IN)
	{
		if (m_SceneOld && m_StackOp != STACK_OP::PUSH) {
			m_SceneOld->Finalize();
		}
		if (m_SceneNext) {
			if (m_StackOp != STACK_OP::POP && !m_HasInitializedNextScene) {
				m_SceneNext->Initialize();
				m_HasInitializedNextScene = true;
			}
		}
		if (m_TransMode == TRANS_MODE::FADE && m_SceneNext)
		{
			DrawNextScene();
		}
		m_isChange = true;
	}
	if (phase == PHASE::FINISH)
	{
		m_Step = STEP::FINISH;
	}

	if (m_Step == STEP::FINISH)
	{
		instance.SetSceneCurrent(m_SceneNext);
		Finalize();
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
	delete this;
	
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
