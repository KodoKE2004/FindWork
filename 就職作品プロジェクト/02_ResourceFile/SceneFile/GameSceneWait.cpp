#include "GameSceneWait.h"
#include "Game.h"

void GameSceneWait::Initialize()
{
}

void GameSceneWait::Update(float tick)
{
    // 乱数生成、ステージを厳選・二回連続で同じステージは踏まない
    int randStageNo = 0;

    switch(randStageNo)
    {
    case SCENE_SLICE:   
    {
        
    }
    break;
    case SCENE_PUSH :   
    {
        
    }
    break;
    case SCENE_HIT  :   
    {
        
    }
    break;

    }

}

void GameSceneWait::Finalize()
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
