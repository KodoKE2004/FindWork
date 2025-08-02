#include"SceneList.h"
#include "../Game.h"
#include "../ObjectFile/Cube.h"
#include "../Texture2D.h"
#include "../ObjectFile/Model.h"

void SelectScene::Initialize()
{
	Model* pt2 = Game::GetInstance().AddObject<Model>();
	pt2->GetMeshModel("RedMan");
	pt2->SetPos(0.0f, 0.0f, 0.0f);
	pt2->SetRotate(0.0f, 0.0f, 0.0f);
	pt2->SetScale(1.0f, 1.0f, 1.0f);
	m_MySceneObjects.emplace_back(pt2);
}

void SelectScene::Update()
{
	if (Input::GetKeyTrigger(VK_RETURN))
	{
		ChangeScene<TitleScene>();
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
