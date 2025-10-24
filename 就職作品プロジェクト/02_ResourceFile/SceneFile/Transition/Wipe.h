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

   
public:
    using TransitionBase::TransitionBase; // Camera* を親から

    void Initialize() override;
    void Update()     override; // ロジックは TransScene 側で進める
    void Draw()       override;
    void Finalize()   override;

};
