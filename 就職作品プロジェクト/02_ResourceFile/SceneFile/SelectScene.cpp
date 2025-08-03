#include"SceneList.h"
#include "SceneTrans.h"

#include "../Game.h"
#include "../DebugUI.h"
#include "../Texture2D.h"
#include "../ObjectFile/Model.h"

void SelectScene::Initialize()
{
	DebugUI::TEXT_CurrentScene = "SelectScene";
	Model* pt2 = Game::GetInstance().AddObject<Model>();
	pt2->GetMeshModel("RedMan");
	m_MySceneObjects.emplace_back(pt2);
}

void SelectScene::Update()
{
	
	if (Input::GetKeyTrigger(VK_RETURN))
	{
		
	}
}

void SelectScene::Finalize()
{
	// このシーンのオブジェクトを削除する
	for (auto& o : m_MySceneObjects) {
		Game::GetInstance().DeleteObject(o);
	}
	m_MySceneObjects.clear();
}
