#include "GameScene.h"
#include "SceneList.h"
#include "../input.h"
#include "../DebugUI.h"
#include "../Game.h"
#include "../ObjectFile/Model.h"

void GameScene::Initialize()
{
    DebugUI::TEXT_CurrentScene = "GameScene";
    auto* pt2 = Game::GetInstance().AddObject<Model>();
    pt2->GetMeshModel("space");
    pt2->SetShader("VS_Unlit", "PS_Unlit");
    pt2->EnableSkyDome(1000.0f);
    m_MySceneObjects.emplace_back(pt2);
}

void GameScene::Update()
{
    if (Input::GetKeyTrigger(VK_RETURN)) {
        ChangeScene<TitleScene>(FADE, 0.5f);
    }
}

void GameScene::Finalize()
{
    for (auto& o : m_MySceneObjects) {
        Game::GetInstance().DeleteObject(o);
    }
    m_MySceneObjects.clear();
}
