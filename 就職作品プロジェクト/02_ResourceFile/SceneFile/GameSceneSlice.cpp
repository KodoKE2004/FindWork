#include "GameSceneSlice.h"
#include "Game.h"

void GameSceneSlice::Initialize()
{
    m_Skydome = GAME_INSTANCE.AddObject<Skydome>();
    m_Skydome->SetName("m_Skydome");
    m_Skydome->SetSkyDomeMode(true);
    m_Skydome->SetTexture(GAME_MANAGER_TEXTURE->GetTexture("SkydomeNight.png"));
    m_Skydome->SetRadius(500.0f);
    m_MySceneObjects.emplace_back(m_Skydome);

}

void GameSceneSlice::Update(float tick)
{

    GameSceneExe::Update(tick);
}

void GameSceneSlice::Finalize()
{
}
