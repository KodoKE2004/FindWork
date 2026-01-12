#pragma once
#include "GameSceneExe.h"
#include "Player.h"
#include "Car.h"
#include "CartWarning.h" 

#include <array>

class GameSceneJump : public GameSceneExe
{
private:
    std::shared_ptr<Cart>        m_Cart;
    std::shared_ptr<CartWarning> m_CartWarning;

    TimerData m_TimeCartActivetion = { 0.0f, 0.0f };
    TimerData m_TimeCartWarning    = { 0.0f, 0.5f };

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

