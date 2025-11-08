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
    m_MySceneObjects.clear();
}
