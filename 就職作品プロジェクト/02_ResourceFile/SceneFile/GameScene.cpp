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
    auto pressTexture = GAME_INSTANCE.AddObject<Texture2D>();
    pressTexture->SetTexture(GAME_MANAGER_TEXTURE->GetTexture("PressToSpace.png"));
    pressTexture->SetPos(0.0f, -250.0f, -1.0f);
    pressTexture->SetScale(400.0f, 300.0f, 1.0f);
    pressTexture->SetUV(1.0f, 1.0f, 1.0f, 1.0f);
    m_MySceneObjects.emplace_back(pressTexture);


}

void GameScene::Update(float tick)
{
    if (Input::GetKeyTrigger(VK_RETURN)) {
        ChangeScene<TitleScene>(FADE, 0.5f);
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
