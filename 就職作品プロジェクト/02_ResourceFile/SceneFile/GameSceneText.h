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

    pShared<Square> m_Boy;
    pShared<Square> m_Girl;

    std::array<pShared<Button>, MESSAGE_SLOT::SLOT_SIZE> m_MessageSlot {nullptr, nullptr, nullptr};
    std::array<size_t, MESSAGE_SLOT::SLOT_SIZE> m_Number     = { 0, 0, 0};
    std::array<float,  MESSAGE_SLOT::SLOT_SIZE> m_GameRhythm = { 0.0f, 0.0f, 0.0f };    // リズムを格納する配列
    std::array<bool ,  MESSAGE_SLOT::SLOT_SIZE> m_Clicked    = { false, false, false }; // クリックされたかどうかの配列
    MESSAGE_SLOT m_SelectedSlot = SLOT_SIZE;

    std::array<pShared<Audio>, 3> m_ReactionAudio;
    pShared<Audio> m_ReactionActive;
    size_t m_CurrentRhythmIndex = 0;    // 現在のリズムインデックス
    size_t m_InputIndex = 0;            // 入力されたインデックス
    float m_JudgeWindow = 0.1f;         // ジャッジウィンドウの許容範囲
    float m_UvXOffset = 0.0f;           // UVのX座標オフセット
    float m_UvXCount = 2.0f;            // UVのX座標カウント
    bool m_isReaction = false;          // slotへの入力開始フラグ
    bool m_isInputAll = false;          // すべてのslotの入力が終わったか


private:
    void ShuffleSlotTextureUV();
    void GirlReaction();
    void ReactionSE(int i,std::string audioName);
public:
    GameSceneText() = delete;
    GameSceneText(Camera& cam);
    ~GameSceneText() = default;

    void Initialize()       override;
    void Update(float tick) override;
    void Draw() 			override;
    void Finalize()         override;

    SCENE_NO GetSceneNo() {
        return SCENE_NO::GAME_TEXT;
    }
    void InsideButton(int i, pShared<Button> button, const MESSAGE_SLOT comparison);
};
