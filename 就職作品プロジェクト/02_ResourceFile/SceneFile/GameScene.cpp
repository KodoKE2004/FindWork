#include "GameScene.h"
#include "SceneList.h"
#include "../input.h"
#include "../DebugUI.h"
#include "../Game.h"
#include "../ObjectFile/Skydome.h"

void GameScene::Initialize()
{
    DebugUI::TEXT_CurrentScene = "GameScene";
    // Create skydome that automatically renders as background sky
    auto* sky = Game::GetInstance().AddObject<Skydome>();
    m_MySceneObjects.emplace_back(sky);
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
