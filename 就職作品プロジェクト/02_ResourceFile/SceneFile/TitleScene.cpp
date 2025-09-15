#include "SceneList.h"
#include "../Game.h"
#include "../DebugUI.h"
#include "../ObjectFile/Cube.h"
#include "../ObjectFile/Light.h"
#include "../Texture2D.h"
#include "../ObjectFile/Model.h"


void TitleScene::Initialize()
{
    DebugUI::TEXT_CurrentScene = "TitleScene";

    auto titleTexture = GAME_INSTANCE.AddObject<Texture2D>();
    titleTexture->SetTexture(GAME_MANAGER_TEXTURE->GetTexture("Title.png"));
	titleTexture->SetShader("VS_Default", "PS_Default");
	titleTexture->SetPos(0.0f, 0.0f, 0.0f);
	titleTexture->SetScale(800.0f, 600.0f, 1.0f);
	titleTexture->SetUV(1.0f, 1.0f, 1.0f, 1.0f);
    m_MySceneObjects.emplace_back(titleTexture);

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

