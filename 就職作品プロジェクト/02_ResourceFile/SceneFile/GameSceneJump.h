#pragma once
#include "GameSceneExe.h"
#include "Player.h"
#include "Bird.h"

#include <array>

class GameSceneJump : public GameSceneExe
{
private:
    
    std::shared_ptr<Bird> m_Bird;

    bool  m_HasSpawnedCartWarning = false;

    size_t m_CurrentCartPatternIndex = 0;

    float GenerateActivationDelay();
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

