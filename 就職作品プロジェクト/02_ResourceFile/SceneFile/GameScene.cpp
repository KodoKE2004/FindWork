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
    GAME_INSTANCE.m_Grid.Initialize(*GAME_INSTANCE.GetShaderManager());

    auto* sky = Game::GetInstance().AddObject<Model>();
    sky->GetMeshModel("space");
    sky->SetScale(50.0f);
    sky->SetTexture(GAME_MANAGER_TEXTURE->GetTexture("space.png"));
    m_MySceneObjects.emplace_back(sky);
}

void GameScene::Update(float tick)
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
