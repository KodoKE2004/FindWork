#pragma once
#include "GameSceneExe.h"
#include "Player.h"
#include "Bird.h"
#include "Stone.h"

#include <vector>

class GameSceneJump : public GameSceneExe
{
private:
    
    std::shared_ptr<Bird>  m_Bird;
    std::vector<std::shared_ptr<Stone>> m_StoneList;

    int m_Beat;
    size_t m_CurrentCartPatternIndex = 0;

    public:
    GameSceneJump()  = default;
    ~GameSceneJump() = default;

    void Initialize()       override;
    void Update(float tick) override;
    void Finalize()         override;


    SCENE_NO GetSceneNo(){
        return SCENE_NO::GAME_JUMP;
    }
};

