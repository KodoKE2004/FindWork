#include"SceneList.h"

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

void SelectScene::Update(float tick)
{
	if (isEnd)
	{
		ChangeScene<GameScene>(FADE, 0.5f);
		return;
    }
	if (isStart)
	{
        m_Time += tick;
		if (m_Time >= m_ChangeTime)
		{
            isEnd = true;
		}
	}
	else
	{
        m_ReadyTime -= tick;
		if (m_ReadyTime <= 0.0f)
		{
            isStart = true;
		}
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
