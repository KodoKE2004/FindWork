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

    enum class WIPE_PHASE 
    {
        WIPE_OUT,
        WIPE_IN,
        FINISH
    };

    WIPE_MODE m_Mode = WIPE_MODE::LEFT_TO_RIGHT;
    WIPE_PHASE m_Phase = WIPE_PHASE::WIPE_OUT;
    float m_Rate = 0.0f; // 0→1 の進行度

    void ApplyWipeAmount(float amount);

public:
    Wipe(Camera* cam);

    void Initialize() override;
    void Update()     override;
    void Draw()       override;
    void Finalize()   override;

    void WIPE_IN();
    void WIPE_OUT();

    // --- 方向指定の簡易Setter（外部から安全に設定） ---
    void SetLeftToRight() { m_Mode = WIPE_MODE::LEFT_TO_RIGHT; }
    void SetRightToLeft() { m_Mode = WIPE_MODE::RIGHT_TO_LEFT; }
    void SetTopToBottom() { m_Mode = WIPE_MODE::TOP_TO_BOTTOM; }
    void SetBottomToTop() { m_Mode = WIPE_MODE::BOTTOM_TO_TOP; }

    float GetRate() const { return m_Rate; }
};