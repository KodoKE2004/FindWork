#include "GameSceneSlice.h"
#include "Game.h"
#include "Collider.h"
#include "Debug.hpp"

void GameSceneSlice::Initialize()
{
    auto& instance = GAME_INSTANCE;
    auto textureMgr = GAME_MANAGER_TEXTURE;

    m_Skydome = instance.AddObject<Skydome>();
    m_Skydome->SetName("m_Skydome");
    m_Skydome->SetSkyDomeMode(true);
    m_Skydome->SetTexture(textureMgr->GetTexture("SkydomeNight.png"));
    m_Skydome->SetRadius(500.0f);
    m_MySceneObjects.emplace_back(m_Skydome);

    m_Sord = instance.AddObject<Player>();
    m_Sord->SetName("m_Sord");
    m_Sord->SetTexture(textureMgr->GetTexture("Sord.png"));
    m_Sord->SetScale  ( 100.0f, 100.0f, 100.0f);
    m_MySceneObjects.emplace_back(m_Sord);


}

void GameSceneSlice::Update(float tick)
{
    using namespace Math::Collider2D;
    m_Sord->m_HitResult.SetHitResult(false
        // isHitSquareSquare(*m_Sord, *m_WoodBefore)
    );

    if (m_Sord->m_HitResult.isTriggered()){
        MyDebugLog(Debug::Log("当たった");)
    }

    m_Sord->Update();
    GameSceneExe::Update(tick);
}

void GameSceneSlice::Finalize()
{
    auto& instance = GAME_INSTANCE;
#ifdef _DEBUG
    instance.m_Grid.SetEnabled(false);
#endif

    // このシーンのオブジェクトを削除する
    for (auto o : m_MySceneObjects) {
        instance.DeleteObject(o);
    }
    m_MySceneObjects.clear();
    // オーディオの停止
    if (auto audioManager = GAME_MANAGER_AUDIO)
    {
        for (const auto& [key, config] : m_AudioList)
        {
            audioManager->StopAllByName(key);
        }
    }
}

void GameSceneSlice::WoodChange()
{
    
}
