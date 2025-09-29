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
#ifdef _DEBUG
	GAME_INSTANCE.m_Grid.SetEnabled(true);
#endif
	auto* m_SkyDome = GAME_INSTANCE.AddObject<Sphere>();
	m_SkyDome->SetSkyDomeMode(true);
	m_SkyDome->SetTexture(GAME_MANAGER_TEXTURE->GetTexture("Plane.png"));
	m_SkyDome->SetRadius(500.0f);
	m_MySceneObjects.emplace_back(m_SkyDome);


}

void TitleScene::Update(float tick)
{
	if (Input::GetKeyTrigger(VK_RETURN))
	{
		Game::GetInstance().GetAudioManager()->Play("enter");

		ChangeScene<GameScene>(FADE, 1.0f);
	}




}



void TitleScene::Finalize()
{

#ifdef _DEBUG
	GAME_INSTANCE.m_Grid.SetEnabled(false);
#endif
	// このシーンのオブジェクトを削除する
	for (auto& o : m_MySceneObjects) {
		Game::GetInstance().DeleteObject(o);
	}
	m_MySceneObjects.clear();
}

