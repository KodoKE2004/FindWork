#include "SceneList.h"
#include "SceneTrans.h"
#include "../Game.h"
#include "../DebugUI.h"
#include "../ObjectFile/Cube.h"
#include "../Texture2D.h"
#include "../ObjectFile/Model.h"

void TitleScene::Initialize()
{
	Cube* pt = Game::GetInstance().AddObject<Cube>();
	m_MySceneObjects.emplace_back(pt);

	DebugUI::TEXT_CurrentScene = "TitleScene";
	Model* pt2 = Game::GetInstance().AddObject<Model>();
	pt2->GetMeshModel("Pokemon");
	m_MySceneObjects.emplace_back(pt2);

	//Texture2D* pt3 = Game::GetInstance().AddObject<Texture2D>();
	//pt3->SetTexture("01_AssetFile/Texture/background1.png");
	//pt3->SetScale ( 100.0f, 100.0f, 100.0f);
}

void TitleScene::Update()
{








}



void TitleScene::Finalize()
{
	// このシーンのオブジェクトを削除する
	for (auto& o : m_MySceneObjects) {
		Game::GetInstance().DeleteObject(o);
	}
	m_MySceneObjects.clear();
}

