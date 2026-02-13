#include "TransScene.h"

#include <memory>

#include "SceneList.h"
#include "main.h"
#include "Game.h"

#include "Fade.h"
#include "Wipe.h"
#include "Zoom.h"


TransScene::TransScene(Camera& cam)
	: Scene(cam)
	, m_Step(STEP::OFF)
	, m_TransMode(TRANS_MODE::FADE)
{
}

TransScene::~TransScene()
{
}

void TransScene::Initialize()
{
    auto& instance = Game::GetInstance();

	// 初期化
	m_Timer = 0.0f;
	m_Alpha = 0.0f;
	m_isChange = false;

	m_Duration = max(m_transParam.duration, 0.0001f);
	m_TransMode = m_transParam.mode;
	m_Step = STEP::DOING;

	const SCENE_NO oldSceneNo  = GetOldSceneNo();
	const SCENE_NO nextSceneNo = GetNextSceneNo();

	
	const bool isOldGameScene  = oldSceneNo == SCENE_NO::GAME_WAIT  ||
								(oldSceneNo >= SCENE_NO::GAME_SLICE &&
								 oldSceneNo  < SCENE_NO::EXE_NUM);

	const bool isNextGameScene = nextSceneNo == SCENE_NO::GAME_WAIT  ||
								(nextSceneNo >= SCENE_NO::GAME_SLICE &&
								 nextSceneNo <  SCENE_NO::EXE_NUM);

	if (isOldGameScene && isNextGameScene) {
        m_isGamePlaying = true;
	}
	else {
        m_isGamePlaying = false;
	}

	const auto& relationData = m_SceneOld->GetRelationData();
	auto applyTransitionTexture = [&](const std::shared_ptr<TransitionBase>& transition)
	{
		if (!transition) {
			return;
		}
		if (relationData.transTexture) 
		{
            auto temp = relationData.transTexture;
            transition->SetTransform(temp->GetTransform());
            transition->SetColor	(temp->GetColor());
            transition->SetTexture	(temp->GetTexture());
			transition->SetUV( temp->GetUV().x, temp->GetUV().y, temp->GetSplit().x, temp->GetSplit().y);
		}
	};
	
	switch (m_TransMode)
	{
	case TRANS_MODE::FADE:
	{
        m_TransitionTexture = std::make_shared<Fade>(instance.GetCamera());
        m_TransitionTexture->SetTransitionParams(m_transParam);
		m_TransitionTexture->Initialize();
        m_TransitionTexture->SetTransMode(m_TransMode);
		instance.AddTransitionTexture(m_TransitionTexture);
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

		applyTransitionTexture(m_TransitionTexture);
		instance.AddTransitionTexture(m_TransitionTexture);
	}
	break;
    case TRANS_MODE::ZOOM_IN:
	{
		m_TransitionTexture = std::make_shared<Zoom>(instance.GetCamera());
		m_TransitionTexture->SetDuration(m_Duration);
		m_TransitionTexture->Initialize();
		m_TransitionTexture->SetTransMode(m_TransMode);
        m_TransitionTexture->SetPhase(TRANS_PHASE::TRANS_IN);
		applyTransitionTexture(m_TransitionTexture);
		instance.AddTransitionTexture(m_TransitionTexture);
	}
    break;
	case TRANS_MODE::ZOOM_OUT:
	{
		m_TransitionTexture = std::make_shared<Zoom>(instance.GetCamera());
		m_TransitionTexture->SetDuration(m_Duration);
		m_TransitionTexture->Initialize();
		m_TransitionTexture->SetTransMode(m_TransMode);
		m_TransitionTexture->SetPhase(TRANS_PHASE::TRANS_OUT);
		applyTransitionTexture(m_TransitionTexture);
    }
	break;
	}

}

void TransScene::Update(float tick)
{
    auto& instance = Game::GetInstance();
    auto& rhythmBeat = instance.GetRhythmBeat();
	if (instance.IsTickCount()) {
        // ゲームシーン間の遷移なら拍のタイマーも更新
        rhythmBeat.TickCount(tick);
	}

	if (m_TransitionTexture == nullptr) {
		return;
	}

	const auto phase = m_TransitionTexture->GetPhase();
    
	if (!m_SceneOld) {
        m_Step = STEP::FINISH;
		instance.SetSceneCurrent(m_SceneNext);
		return;
	}

	m_TransitionTexture->Update(tick);

	if (!m_isChange && m_TransitionTexture->IsChange())
	{
		m_SceneOld->Finalize();
		if (m_SceneNext) {
			m_SceneNext->Initialize();
			m_RequestNextSceneDraw = true;
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

void TransScene::Draw()
{
	if (m_RequestNextSceneDraw) {
		if (m_SceneNext) {
			m_SceneNext->Draw();
		}
	}
	else
	{
		if (m_SceneOld) {
			m_SceneOld->Draw();
		}
	}

}

void TransScene::Finalize()
{
    auto& instance = Game::GetInstance();
	m_Step = STEP::OFF;
	for (auto obj : m_MySceneObjects)
	{
		DeleteObject(obj);
	}
	m_MySceneObjects.clear();

	m_OverlayNext.reset();
	m_TransitionTexture = nullptr ;
    instance.ClearTransitionTexture();
	m_RequestNextSceneDraw = false;

}

