#include "GameScenePush.h"
#include "Game.h"

void GameScenePush::Initialize()
{
    auto& instance = Game::GetInstance();
    TextureManager* textureMgr = instance;

    m_Player = instance.AddObject<Player>(instance.GetCamera());
    m_Player->SetPos(NVector3(0.0f, -2.0f, 0.0f));
    m_Player->SetScale(NVector3(100.0f, 100.0f, 100.0f));
    m_Player->SetTexture(textureMgr->GetTexture("Plane.png"));
    m_MySceneObjects.emplace_back(m_Player);

    m_Cart = instance.AddObject<Cart>(instance.GetCamera());
    m_Cart->SetPos(NVector3(0.0f, 2.0f, 0.0f));
    m_Cart->SetScale(NVector3(100.0f, 100.0f, 100.0f));
    m_Cart->SetTexture(textureMgr->GetTexture("Plane.png"));
    m_MySceneObjects.emplace_back(m_Cart);
}

void GameScenePush::Update(float tick)
{
    GameSceneExe::Update(tick);
}

void GameScenePush::Finalize()
{
    GameSceneExe::Finalize();
}
