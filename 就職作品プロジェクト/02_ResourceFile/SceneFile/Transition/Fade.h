#pragma once
#include "TransitionBase.h"

class Fade : public TransitionBase
{
private:

    float m_Alpha = 0.0f;     
    float m_Elapsed = 0.0f;  

public:
    explicit Fade(Camera* cam);
    void Initialize()       override;  
    void Update(float tick) override;      
    void Draw()             override;        
    void Finalize()         override;    

    void FADE_IN (float tick);
    void FADE_OUT(float tick);

    void ApplyAlpha();

    void SetTransMode(TRANS_MODE transMode) override;
};