#pragma once
#include "GameSceneExe.h"
#include "Button.h"
#include <memory>
#include <array>

class GameSceneText : public GameSceneExe
{
private:
    const NVector3 BUTTTON_BASE_SCALE = NVector3(240.0f, 100.0f, 1.0f);

    enum MESSAGE_SLOT
    {
        ADVERB,
        ADJECTIVE_A,
        ADJECTIVE_B,
        SLOT_SIZE
    };

    std::shared_ptr<Square> m_Boy;
    std::shared_ptr<Square> m_Girl;

    std::array<std::shared_ptr<Button>, MESSAGE_SLOT::SLOT_SIZE> m_MessageSlot {nullptr, nullptr, nullptr};
    std::array<size_t, MESSAGE_SLOT::SLOT_SIZE> m_Number     = { 0, 0, 0};
    std::array<float,  MESSAGE_SLOT::SLOT_SIZE> m_GameRhythm = { 0.0f, 0.0f, 0.0f };    // リズムを格納する配列
    std::array<bool ,  MESSAGE_SLOT::SLOT_SIZE> m_Clicked    = { false, false, false }; // クリックされたかどうかの配列

    float m_Elapsed = 0.0f;
    size_t m_CurrentRhythmIndex = 0;
    size_t m_InputIndex = 0;
    float m_JudgeWindow = 0.1f;
    float m_UvXOffset = 0.0f;

    bool m_isEntry = false;
    bool m_isInputSlot = false; // slotの入力受付フラグ
    bool m_isInputAll = false;  // すべてのslotの入力が終わったか

private:
    void RhythmJudge(float );
    void ShuffleSlotTextureUV();
    void GirlReaction();
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
