#include "GameSceneSlice.h"
#include "Game.h"

void GameSceneSlice::Initialize()
{
    auto& instance = GAME_INSTANCE;

    m_Skydome = GAME_INSTANCE.AddObject<Skydome>();
    m_Skydome->SetName("m_Skydome");
    m_Skydome->SetSkyDomeMode(true);
    m_Skydome->SetTexture(GAME_MANAGER_TEXTURE->GetTexture("SkydomeNight.png"));
    m_Skydome->SetRadius(500.0f);
    m_MySceneObjects.emplace_back(m_Skydome);

    m_Sord = instance.AddObject<Square>();
    m_Sord->SetName("m_Sord");
    m_Sord->SetScale( 10.0f, 10.0f, 10.0f);
    m_MySceneObjects.emplace_back(m_Sord);
}

void GameSceneSlice::Update(float tick)
{
    
    GameSceneExe::Update(tick);
}

void GameSceneSlice::Finalize()
{
}
