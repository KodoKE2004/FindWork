#include "TransScene.h"
#include "SceneList.h"
#include "main.h"
#include "Game.h"
#include <memory>

TransScene::TransScene()
	: m_SceneOld (nullptr)
	, m_SceneNext(nullptr)
	, m_Step(STEP::OFF)
	, m_TransMode(TRANS_MODE::FADE)
{
}

void TransScene::Initialize()
{
    auto& instance = GAME_INSTANCE;

	m_Timer = 0.0f;
	m_Alpha = 0.0f;
	m_isChange = false;
	m_hasInitializedNextScene = false;
	if (m_Duration <= 0.0f) {
		m_Duration = 1.0f;
	}
	m_Step = STEP::DOING;
	
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
			m_hasInitializedNextScene = true;
		}

		if (m_SceneNext) {
			DrawNextScene();
		}

		m_TransitionTexture->SetTextureSRV(m_NextSceneSRV.Get());
		m_TransitionTexture->SetPos(0.0f, 0.0f, -2.0f);
		instance.SetTransitionTexture	 (m_TransitionTexture);
	}
	break;
	}

    m_TransitionTexture->SetPhase	 (PHASE::TRANS_OUT);
	m_TransitionTexture->SetTimerInfo(0.0f, m_Duration);

}

void TransScene::Update(float tick)
{
    auto& instance = GAME_INSTANCE;

	if (m_TransitionTexture == nullptr) {
		return;
	}

	m_TransitionTexture->Update();

	const auto phase = m_TransitionTexture->GetPhase();
	if (!m_isTransOutToIn && phase == PHASE::TRANS_IN)
	{
		
		m_isTransOutToIn = true;
	}

	if (!m_isChange && phase == PHASE::TRANS_IN)
	{
		
		m_SceneOld->Finalize();
		m_SceneNext->Initialize();
		m_hasInitializedNextScene = true;
		DrawNextScene();
		
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
		Game::GetInstance().DeleteObject(obj.get());
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
	auto& instance = GAME_INSTANCE;

	auto* device  = Renderer::GetDevice();
	auto* context = Renderer::GetDeviceContext();
	auto  vp	  = Renderer::GetViewport();

	m_RenderTarget = std::make_unique<RenderTarget>();

	m_RenderTarget->Create(device, (UINT)vp.Width, (UINT)vp.Height, true);

	const float clear[4]{ 0, 0, 0, 0 };
	m_RenderTarget->Begin(context, clear);
	{

		for (auto obj : m_SceneNext->GetSceneObjects()) {
			if (obj) obj->Draw();
		}
	}
	RenderTarget::End(context, Renderer::GetBackBufferRTV(), vp);

	m_NextSceneSRV = m_RenderTarget->GetSRV();


	m_Overlay = instance.AddObject<SnapshotOverlay>();
	m_Overlay->SetSRV(m_NextSceneSRV.Get());
	m_Overlay->SetAlpha(0.0f);
	m_MySceneObjects.emplace_back(m_Overlay);

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