#include "GameScene.h"
#include "SceneList.h"
#include "../input.h"
#include "../DebugUI.h"
#include "../Game.h"
#include "../ObjectFile/Model.h"
#include "../ObjectFile/Skydome.h"

void GameScene::Initialize()
{
    DebugUI::TEXT_CurrentScene = "GameScene";
#ifdef _DEBUG
    GAME_INSTANCE.m_Grid.SetEnabled(true);
#endif

}

void GameScene::Update(float tick)
{
    if (Input::GetKeyTrigger(VK_RETURN)) {
        ChangeScenePush<TitleScene>(TRANS_MODE::FADE, 0.5f);
    }
}

void GameScene::Finalize()
{
#ifdef _DEBUG
    GAME_INSTANCE.m_Grid.SetEnabled(false);
#endif
    for (auto& o : m_MySceneObjects) {
        Game::GetInstance().DeleteObject(o);
    }
    m_MySceneObjects.clear();
}
