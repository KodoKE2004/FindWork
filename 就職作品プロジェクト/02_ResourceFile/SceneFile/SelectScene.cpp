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
    auto& instance = Game::GetInstance();

	DebugUI::TEXT_CurrentScene = "SelectScene";
    TextureManager* textureMgr = instance;
	
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

    m_Cursor = instance.AddObject<Square>();
    m_Cursor->SetName("m_Corsor");
    m_Cursor->SetPos(m_ButtonGamePlay->GetPos());
    m_Cursor->SetScale(450.0f, 100.0f, 1.0f);
    m_Cursor->SetShader("VS_Alpha", "PS_Alpha");
    m_Cursor->SetTexture(textureMgr->GetTexture("Black.png"));
	m_Cursor->SetColor(0.0f, 0.0f, 0.0f, 0.3f);
    m_MySceneObjects.emplace_back(m_Cursor);

}

void SelectScene::Update(float tick)
{
	if (Input::GetKeyTrigger(VK_W)) {
        m_Cursor->SetPos(m_ButtonGamePlay->GetPos());
	}
	if (Input::GetKeyTrigger(VK_S)) {
        m_Cursor->SetPos(m_ButtonPractice->GetPos());
	}

	if (Input::GetKeyTrigger(VK_RETURN)) 
	{
		ChangeScenePush<GameSceneWait>(TitleToWait);
	}	

}

void SelectScene::Finalize()
{
    auto& instance = Game::GetInstance();

	// このシーンのオブジェクトを削除する
	for (auto obj : m_MySceneObjects) {
		instance.DeleteObject(obj);
	}
	m_MySceneObjects.clear();
}
