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

    float m_PlayerGroundHeight    = -100.0f;
    float m_PlayerJumpApexHeight  =  200.0f;
    static constexpr float PlayerJumpHeightScale = 2.5f;

    std::array<CarStartPattern, 4> m_CartStartPatterns{};
    size_t m_CurrentCartPatternIndex = 0;

    void ApplyCartPattern(size_t index);
    void AdvanceCartPattern();
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

