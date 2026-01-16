#pragma once
#include "GameSceneExe.h"
#include "Button.h"
#include <memory>

class GameSceneText : public GameSceneExe
{
private:
    const NVector3 BUTTTON_BASE_SCALE = NVector3(240.0f, 100.0f, 1.0f);

    enum LAST_DRAG
    {
        NONE,
        BUTTON_TRUE,
        BUTTON_FALSE_A,
        BUTTON_FALSE_B,
    };
    std::shared_ptr<Square> m_Boy;
    std::shared_ptr<Square> m_Girl;

    std::shared_ptr<Button> m_Adverb;
    std::shared_ptr<Button> m_AdjectiveA;
    std::shared_ptr<Button> m_AdjectiveB;

    NVector3 m_TrueTargetPos{};
    NVector3 m_FalseATargetPos{};
    NVector3 m_FalseBTargetPos{};

    float m_GameRhythm[3] = { 0.0f, 0.0f, 0.0f };    // ƒŠƒYƒ€‚ðŠi”[‚·‚é”z—ñ

    float m_ButtonFadeTimer = 0.0f;
    float m_ButtonFadeDuration;

    bool m_HasSpawnedCartWarning = false;
    bool trigger = false;

    LAST_DRAG m_SelectedButton;

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

    void InsideButton(LAST_DRAG& lastDrag, std::weak_ptr<Button> button, const LAST_DRAG comparison);
};
