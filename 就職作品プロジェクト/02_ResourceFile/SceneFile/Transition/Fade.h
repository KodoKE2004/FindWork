#pragma once
#include "TransitionBase.h"


class Fade : public TransitionBase
{
private:
    // フェードの段階: 画面を黒で覆う(OUT) → 黒から戻す(IN) → 完了
    float m_Alpha = 0.0f;       // 現在アルファ
    float m_AlphaValue = 0.0f;  // 秒あたり増減量（1 / duration）

public:
    explicit Fade(Camera* cam);
    void Initialize() override;     // リソース初期化、パラメータ初期化
    void Update() override;         // フェーズに応じて進行
    void Draw() override;           // フルスクリーンの黒板を描画
    void Finalize() override;       // 今は何もしない

    // 1フレーム分の処理: 黒→表示（IN）、表示→黒（OUT）
    void FADE_IN();
    void FADE_OUT();

    // m_Alpha を [0,1] にクランプして色へ反映
    void ApplyAlpha();
};