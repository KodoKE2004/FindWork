#include "TransScene.h"

#include <memory>

#include "Game.h"
#include "main.h"
#include "SceneList.h"

#include "Fade.h"
#include "Wipe.h"
#include "Zoom.h"


TransScene::TransScene()
	: m_Step(STEP::OFF)
	, m_TransMode(TRANS_MODE::FADE)
{
}

TransScene::~TransScene()
{
}

void TransScene::Initialize()
{
    auto& instance = Game::GetInstance();

	// èâä˙âª
	m_Timer = 0.0f;
	m_Alpha = 0.0f;
	m_isChange = false;

	m_Duration = max(m_transParam.duration, 0.0001f);
	m_TransMode = m_transParam.mode;
	m_Step = STEP::DOING;
	m_AlphaValue = 1.0f / m_Duration;

	
	
	switch (m_TransMode)
	{
	case TRANS_MODE::FADE:
	{
        m_TransitionTexture = std::make_shared<Fade>(instance.GetCamera());
        m_TransitionTexture->SetTransitionParams(m_transParam);
		m_TransitionTexture->Initialize();
        m_TransitionTexture->SetTransMode(m_TransMode);

		instance.SetTransitionTexture(m_TransitionTexture);
	}
	break;
	case TRANS_MODE::WIPE_BOTTOM_TO_TOP:
	case TRANS_MODE::WIPE_LEFT_TO_RIGHT:
	case TRANS_MODE::WIPE_RIGHT_TO_LEFT:
	case TRANS_MODE::WIPE_TOP_TO_BOTTOM:
	{

		m_TransitionTexture = std::make_shared<Wipe>(instance.GetCamera());
		m_TransitionTexture->SetTransitionParams(m_transParam);
		m_TransitionTexture->Initialize();
        m_TransitionTexture->SetTransMode(m_TransMode);
		
		instance.SetTransitionTexture(m_TransitionTexture);
	}
	break;
    case TRANS_MODE::ZOOM_IN:
	case TRANS_MODE::ZOOM_OUT:
	{
		m_TransitionTexture = std::make_shared<Zoom>(instance.GetCamera());
		m_TransitionTexture->SetDuration(m_Duration);
		m_TransitionTexture->Initialize();
		m_TransitionTexture->SetTransMode(m_TransMode);

		instance.SetTransitionTexture(m_TransitionTexture);
	}
	break;
	}

}

void TransScene::Update(float tick)
{
    auto& instance = Game::GetInstance();

	if (m_TransitionTexture == nullptr) {
		return;
	}


	const auto phase = m_TransitionTexture->GetPhase();
    
	if (!m_SceneOld) {
        m_Step = STEP::FINISH;
		return;
	}

	m_TransitionTexture->Update(tick);

	if (!m_isChange && m_TransitionTexture->IsChange())
	{
		m_SceneOld->Finalize();

		if (m_SceneNext) {
            m_SceneNext->Initialize();
            Debug::Log("[[åüèo]] NextScene Initialize");
			DrawNextScene();
		}
		m_isChange = true;
	}

	if (phase == TRANS_PHASE::FINISH)
	{
		m_Step = STEP::FINISH;
		Finalize();
		instance.SetSceneCurrent(m_SceneNext);
		return;
	}


}

void TransScene::Finalize()
{
    auto& instance = Game::GetInstance();
	m_Step = STEP::OFF;
	for (auto obj : m_MySceneObjects)
	{
		obj->SetTexture(nullptr);
		instance.DeleteObject(obj);
	}
	m_MySceneObjects.clear();

	m_OverlayNext.reset();
	m_TransitionTexture = nullptr ;
	instance.SetTransitionTexture(nullptr);
	m_NextSceneSRV.Reset();
	m_RenderTarget.reset();

#ifdef _DEBUG
	instance.m_Grid.DeInitialized();
#endif // _DEBUG
	
}

void TransScene::DrawNextScene()
{
	auto& instance  = Game::GetInstance();
	if (!m_SceneNext) {
		return;
	}

	auto* device   = Renderer::GetDevice();
	auto* context  = Renderer::GetDeviceContext();
	auto  viewport = Renderer::GetViewport();

	m_RenderTarget = std::make_unique<RenderTarget>();

	m_RenderTarget->Create(device, (UINT)viewport.Width, (UINT)viewport.Height, true);

	const float clear[4]{ 0, 0, 0, 0 };
	m_RenderTarget->Begin(context, clear);
	{
		for (auto obj : m_SceneNext->GetSceneObjects()) {
			if (obj) obj->Draw();
		}
	}
	
	m_RenderTarget->End(context);

	m_NextSceneSRV = m_RenderTarget->GetSRV();


	m_OverlayNext = instance.AddObject<SnapshotOverlay>();
	m_OverlayNext->SetSRV(m_NextSceneSRV.Get());
	m_OverlayNext->SetAlpha(0.0f);
	m_MySceneObjects.emplace_back(m_OverlayNext);

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
