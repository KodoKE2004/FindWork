#pragma once
#include "TransitionBase.h"
#include <wrl/client.h>
#include <d3d11.h>


class Wipe : public TransitionBase
{
private:
    enum class WIPE_MODE 
    {
        LEFT_TO_RIGHT,
        RIGHT_TO_LEFT,
        TOP_TO_BOTTOM,
        BOTTOM_TO_TOP,
        NUM 
     };

    WIPE_MODE m_Mode = WIPE_MODE::LEFT_TO_RIGHT;

    float m_Elapsed = 0.0f; // 経過時間

    NVector3 m_StartPos;
    NVector3 m_EndPos;


    // 進行度毎にワイプ画像の座標を適用
    void ApplyWipeAmount(float amount);
public:
    explicit Wipe(Camera* cam);

    void Initialize()       override;
    void Update(float tick) override;
    void Draw()             override;
    void Finalize()         override;

    void WIPE_IN(float tick);
    void WIPE_OUT(float tick);

    // --- 方向指定の簡易Setter（外部から安全に設定） ---
    void SetLeftToRight() { m_Mode = WIPE_MODE::LEFT_TO_RIGHT; }
    void SetRightToLeft() { m_Mode = WIPE_MODE::RIGHT_TO_LEFT; }
    void SetTopToBottom() { m_Mode = WIPE_MODE::TOP_TO_BOTTOM; }
    void SetBottomToTop() { m_Mode = WIPE_MODE::BOTTOM_TO_TOP; }

    void  SetTransMode(TRANS_MODE transMode) override;

    // ワイプの開始点と終着点の設定
    void CheckPointSetting();
};