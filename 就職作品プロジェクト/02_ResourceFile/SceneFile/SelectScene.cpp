#include"SceneList.h"

#include "Game.h"
#include "DebugUI.h"
#include "Square.h"
#include "ObjectFile/Model.h"
#include "ObjectFile/Bullet.h"
#include "Camera.h"

void SelectScene::Initialize()
{
    auto& instance = GAME_INSTANCE;

	DebugUI::TEXT_CurrentScene = "SelectScene";
    auto textureMgr = GAME_MANAGER_TEXTURE;

#ifdef _DEBUG
	instance.m_Grid.SetEnabled(true);
#endif
	
    m_SkyDome = instance.AddObject<Sphere>();
    m_SkyDome->SetName("m_SkyDome");
	m_SkyDome->SetSkyDomeMode(true);
    m_SkyDome->SetTexture(textureMgr->GetTexture("SkydomeSpace.png"));
	m_SkyDome->SetRadius(500.0f);
	m_MySceneObjects.emplace_back(m_SkyDome);

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
    m_Corsor->SetTexture(textureMgr->GetTexture("Black.png"));
	m_Corsor->SetColor(0.0f, 0.0f, 0.0f, 0.1f);
    m_MySceneObjects.emplace_back(m_Corsor);

}

void SelectScene::Update(float tick)
{
	if      (m_Corsor->GetPos().y == m_ButtonGamePlay->GetPos().y) {
		
	}
	else if (m_Corsor->GetPos().y == m_ButtonPractice->GetPos().y) {

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
