#include "GameScene.h"
#include "SceneList.h"
#include "../input.h"
#include "../DebugUI.h"
#include "../Game.h"
#include "../ObjectFile/Sphere.h"

void GameScene::Initialize()
{
    DebugUI::TEXT_CurrentScene = "GameScene";
    auto* sky = Game::GetInstance().AddObject<Sphere>();
    sky->SetShader("VS_Unlit", "PS_Unlit");
    sky->EnableSkyDome("space.png", 1000.0f);
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
