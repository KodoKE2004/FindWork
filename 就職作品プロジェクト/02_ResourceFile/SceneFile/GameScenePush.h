#pragma once
#include "GameSceneExe.h"
#include "Player.h"
#include "Car.h"

#include <array>

class GameScenePush : public GameSceneExe
{
private:
    Cart*   m_Cart   = nullptr;
    Player* m_Player = nullptr;

    float m_PlayerGroundHeight    = - 100.0f;
    float m_PlayerJumpApexHeight  =   200.0f;
    static constexpr float PlayerJumpHeightScale = 2.5f;


    float m_CartAcitvationTimer = 0.0f;
    float m_CartActivationDelay = 0.0f;

    size_t m_CurrentCartPatternIndex = 0;

    float GenerateActivationDelay();
public:
    GameScenePush()  = default;
    ~GameScenePush() = default;

    void Initialize()       override;
    void Update(float tick) override;
    void Finalize()         override;


    SCENE_NO GetSceneNo(){
        return SCENE_NO::GAME_PUSH;
    }
};

