#include"SceneList.h"

#include "Game.h"
#include "DebugUI.h"
#include "Square.h"
#include "ObjectFile/Model.h"
#include "ObjectFile/Bullet.h"
#include "Camera.h"
#include "input.h"

void SelectScene::Initialize()
{
    auto& instance = GAME_INSTANCE;

	DebugUI::TEXT_CurrentScene = "SelectScene";
    auto textureMgr = GAME_MANAGER_TEXTURE;

#ifdef _DEBUG
	instance.m_Grid.SetEnabled(true);
#endif
	
    m_Skydome = instance.AddObject<Skydome>();
    m_Skydome->SetName("m_SkyDome");
	m_Skydome->SetSkyDomeMode(true);
    m_Skydome->SetTexture(textureMgr->GetTexture("SkydomeSpace.png"));
	m_Skydome->SetRadius(500.0f);
	m_MySceneObjects.emplace_back(m_Skydome);

	m_ButtonGamePlay = instance.AddObject<Square>();
    m_ButtonGamePlay->SetName("m_ButtonGamePlay");
	m_ButtonGamePlay->SetPos(0.0f, -150.0f, 0.0f);
    m_ButtonGamePlay->SetScale(450.0f, 100.0f, 1.0f);
    m_ButtonGamePlay->SetTexture(textureMgr->GetTexture("ButtonGamePlay.png"));
	m_MySceneObjects.emplace_back(m_ButtonGamePlay);

    m_ButtonPractice = instance.AddObject<Square>();
    m_ButtonPractice->SetName("m_ButtonPractice");
	m_ButtonPractice->SetPos(0.0f, -275.0f, 0.0f);
    m_ButtonPractice->SetScale(450.0f, 100.0f, 1.0f);
    m_ButtonPractice->SetTexture(textureMgr->GetTexture("ButtonPractice.png"));
    m_MySceneObjects.emplace_back(m_ButtonPractice);

    m_Corsor = instance.AddObject<Square>();
    m_Corsor->SetName("m_Corsor");
    m_Corsor->SetPos(m_ButtonGamePlay->GetPos());
    m_Corsor->SetScale(450.0f, 100.0f, 1.0f);
    m_Corsor->SetShader("VS_Alpha", "PS_Alpha");
    m_Corsor->SetTexture(textureMgr->GetTexture("Black.png"));
	m_Corsor->SetColor(0.0f, 0.0f, 0.0f, 0.3f);
    m_MySceneObjects.emplace_back(m_Corsor);

}

void SelectScene::Update(float tick)
{
	if (Input::GetKeyTrigger(VK_W)) {
        m_Corsor->SetPos(m_ButtonGamePlay->GetPos());
	}
	if (Input::GetKeyTrigger(VK_S)) {
        m_Corsor->SetPos(m_ButtonPractice->GetPos());
	}

	if (Input::GetKeyTrigger(VK_RETURN)) 
	{
		ChangeScenePush<GameSceneSlice>(TRANS_MODE::FADE,0.01f);
	}

	

	
	

}

void SelectScene::Finalize()
{
#ifdef _DEBUG
	GAME_INSTANCE.m_Grid.SetEnabled(false);
#endif
	// このシーンのオブジェクトを削除する
	for (auto obj : m_MySceneObjects) {
		Game::GetInstance().DeleteObject(obj);
	}
	m_MySceneObjects.clear();
}
