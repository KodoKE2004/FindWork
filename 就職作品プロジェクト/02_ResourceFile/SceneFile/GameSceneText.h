#pragma once
#include "GameSceneExe.h"
#include "Button.h"
#include <memory>
#include <array>

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

    std::array<size_t, 3> m_Number;

    std::array<float,3> m_GameRhythm = { 0.0f, 0.0f, 0.0f };    // リズムを格納する配列
    std::array<bool, 3> m_Clicked    = { false, false, false }; // クリックされたかどうかの配列

    size_t m_PhaseIndex = 0;
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
