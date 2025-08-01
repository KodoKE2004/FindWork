#include"SceneList.h"
#include "../Game.h"
#include "../TestFile/TestCube.h"
#include "../Texture2D.h"
#include "../ModelObject.h"

void TitleScene::Initialize()
{
	TestCube* pt = Game::GetInstance().AddObject<TestCube>();
	m_MySceneObjects.emplace_back(pt);

	ModelObject* pt2 = Game::GetInstance().AddObject<ModelObject>();
	pt2->SetPos   ( 0.0f, 0.0f, 0.0f);
	pt2->SetRotate( 0.0f, 0.0f, 0.0f);
	pt2->SetScale ( 1.0f, 1.0f, 1.0f);

	m_MySceneObjects.emplace_back(pt2);

	//Texture2D* pt3 = Game::GetInstance().AddObject<Texture2D>();
	//pt3->SetTexture("01_AssetFile/Texture/background1.png");
	//pt3->SetScale ( 100.0f, 100.0f, 100.0f);
}

void TitleScene::Update()
{
	if(Input::GetKeyTrigger(VK_RETURN))
	{
		ChangeScene<SelectScene>();
	}
}



void TitleScene::Finalize()
{
	// このシーンのオブジェクトを削除する
	for (auto& o : m_MySceneObjects) {
		Game::GetInstance().DeleteObject(o);
	}
}

