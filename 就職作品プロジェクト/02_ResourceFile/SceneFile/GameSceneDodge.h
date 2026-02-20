#pragma once
#include "GameSceneExe.h"
#include "Player.h"
#include "Bird.h"
#include "Stone.h"

#include <vector>

class GameSceneDodge : public GameSceneExe
{
private:
    pShared<Square> m_Background;
    pShared<Bird>   m_Bird;
    pShared<Stone>  m_Stone;

    float m_StoneSpawnElapsed = 0.0f;
    size_t m_CurrentCartPatternIndex = 0;

    std::vector<pShared<Stone>> m_StoneList;

public:
    GameSceneDodge()  = delete;
    GameSceneDodge(Camera& cam);
    ~GameSceneDodge() = default;

    void Initialize()       override;
    void Update(float tick) override;
    void Draw() 			override;
    void Finalize()         override;


    SCENE_NO GetSceneNo(){
        return SCENE_NO::GAME_DODGE;
    }
};

