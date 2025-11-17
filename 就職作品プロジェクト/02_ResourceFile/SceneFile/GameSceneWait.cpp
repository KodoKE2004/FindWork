#include "GameSceneWait.h"
#include "Game.h"
#include "DebugUI.h"

void GameSceneWait::Initialize()
{
    auto& instance = Game::GetInstance();
    DebugUI::TEXT_CurrentScene = "GameSceneWait";
    SCENE_NO nextScene[3];

    // 初期化の段階で次のシーンを決定しておく
    // 二回目以降のシーンの呼び出しではシーンが連続しないようにする
    // 一回目の判断はstageCountで行う
    if (m_RelationData.stageCount == 0)
    {
        
    }
}

void GameSceneWait::Update(float tick)
{
    // 乱数生成、ステージを厳選・二回連続で同じステージは踏まない





    switch(m_RelationData.nextScene)
    {
    case SCENE_NO::GAME_SLICE:   
    {
        
    }
    break;
    case SCENE_NO::GAME_PUSH:
    {
        
    }
    break;
    case SCENE_NO::GAME_HIT :   
    {
        
    }
    break;

    }

}

void GameSceneWait::Finalize()
{
    auto& instance = Game::GetInstance();
#ifdef _DEBUG
    instance.m_Grid.SetEnabled(false);
#endif

    // このシーンのオブジェクトを削除する
    for (auto o : m_MySceneObjects) {
        instance.DeleteObject(o);
    }
    m_MySceneObjects.clear();
    // オーディオの停止
    if (AudioManager* audioManager = instance)
    {
        for (const auto& [key, config] : m_AudioList)
        {
            audioManager->StopAllByName(key);
        }
    }
}
