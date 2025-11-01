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
        OUT,
        IN,
        FINISH,
    };

    WIPE_MODE  m_Mode  = WIPE_MODE::LEFT_TO_RIGHT;
    WIPE_PHASE m_Phase = WIPE_PHASE::OUT;
    float      m_Rate  = 0.0f;

    void ApplyWipeAmount(float amount);

public:
    using TransitionBase::TransitionBase; // Camera* e

    void Initialize() override;
    void Update()     override;
    void Draw()       override;
    void Finalize()   override;

    void WIPE_IN();
    void WIPE_OUT();
};
