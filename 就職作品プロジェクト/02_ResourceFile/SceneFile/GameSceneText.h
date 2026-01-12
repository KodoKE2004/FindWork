#pragma once
#include "GameSceneExe.h"
class GameSceneText : public GameSceneExe
{
private:

    TimerData m_TimeCartActivetion = { 0.0f, 0.0f };
    TimerData m_TimeCartWarning = { 0.0f, 0.5f };

    bool  m_HasSpawnedCartWarning = false;

    size_t m_CurrentCartPatternIndex = 0;

    float GenerateActivationDelay();
public:
    GameSceneText() = default;
    ~GameSceneText() = default;

    void Initialize()       override;
    void Update(float tick) override;
    void Finalize()         override;

    SCENE_NO GetSceneNo() {
        return SCENE_NO::GAME_TEXT;
    }
};
