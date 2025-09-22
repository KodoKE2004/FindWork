#include "SceneList.h"
#include "../Game.h"
#include "../DebugUI.h"
#include "../ObjectFile/Cube.h"
#include "../ObjectFile/Sphere.h"
#include "../Texture2D.h"
#include "../ObjectFile/Model.h"


void TitleScene::Initialize()
{
    DebugUI::TEXT_CurrentScene = "TitleScene";
	GAME_INSTANCE.m_Grid.Initialize(*GAME_INSTANCE.GetShaderManager());

	auto* m_SkyDome = GAME_INSTANCE.AddObject<Sphere>();
	m_SkyDome->SetSkyDomeMode(true);
	m_SkyDome->SetTexture(GAME_MANAGER_TEXTURE->GetTexture("Plane.png"));
	m_SkyDome->SetRadius(500.0f);
	m_MySceneObjects.emplace_back(m_SkyDome);

    auto titleTexture = GAME_INSTANCE.AddObject<Texture2D>();
    titleTexture->SetTexture(GAME_MANAGER_TEXTURE->GetTexture("Title.png"));
	titleTexture->SetShader("VS_Default", "PS_Default");
	titleTexture->SetPos(0.0f, 0.0f, 0.0f);
	titleTexture->SetScale(800.0f, 600.0f, 1.0f);
	titleTexture->SetUV(1.0f, 1.0f, 1.0f, 1.0f);
    m_MySceneObjects.emplace_back(titleTexture);

    auto pressTexture = GAME_INSTANCE.AddObject<Texture2D>();
    pressTexture->SetTexture(GAME_MANAGER_TEXTURE->GetTexture("PressToSpace.png"));
	pressTexture->SetPos(0.0f, -250.0f, -1.0f);
	pressTexture->SetScale(400.0f, 300.0f, 1.0f);
	pressTexture->SetUV(1.0f, 1.0f, 1.0f, 1.0f);
    m_MySceneObjects.emplace_back(pressTexture);

	Game::GetInstance().GetAudioManager()->Add("enter", L"SE/Enter.wav");
}

void TitleScene::Update(float tick)
{
	if (Input::GetKeyTrigger(VK_RETURN))
	{
		Game::GetInstance().GetAudioManager()->Play("enter");
		ChangeScene<SelectScene>(FADE, 1.0f);
	}




}



void TitleScene::Finalize()
{
	// このシーンのオブジェクトを削除する
	for (auto& o : m_MySceneObjects) {
		Game::GetInstance().DeleteObject(o);
	}
	m_MySceneObjects.clear();
}

