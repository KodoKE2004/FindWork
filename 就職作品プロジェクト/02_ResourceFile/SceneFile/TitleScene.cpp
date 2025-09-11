#include "SceneList.h"
#include "../Game.h"
#include "../DebugUI.h"
#include "../ObjectFile/Cube.h"
#include "../ObjectFile/Light.h"
#include "../Texture2D.h"
#include "../ObjectFile/Model.h"

void TitleScene::Initialize()
{
	//Cube* pt = Game::GetInstance().AddObject<Cube>();
	//pt->SetShader("VS_Unlit", "PS_Unlit");
	//m_MySceneObjects.emplace_back(pt);

	// Light* light = Game::GetInstance().AddObject<Light>();
	// light->ClearShader();
	// light->SetShader("VS_Unlit");
	// light->SetShader("PS_Unlit");
	// light->SetPos(0.0f, 10.0f, 0.0f);
	// m_MySceneObjects.emplace_back(light);

	auto* pt2 = Game::GetInstance().AddObject<Model>();
	pt2->GetMeshModel("Pokemon");
	pt2->SetShader( "VS_Unlit", "PS_Unlit");
	pt2->SetScale ( 1.0f, 1.0f, 1.0f);
	m_MySceneObjects.emplace_back(pt2);

	Game::GetInstance().GetAudioManager()->Add("enter", L"SE/Enter.wav");
}

void TitleScene::Update()
{
	if (Input::GetKeyTrigger(VK_RETURN))
	{
		// Game::GetInstance().GetAudioManager()->Play("enter");
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

