#pragma once
#include "TransitionBase.h"

class Fade : public TransitionBase
{
private:

    float m_Alpha = 0.0f;       
    float m_AlphaValue = 0.0f;  

public:
    explicit Fade(Camera* cam);
    void Initialize() override;  
    void Update() override;      
    void Draw() override;        
    void Finalize() override;    

    void FADE_IN();
    void FADE_OUT();

    void ApplyAlpha();
};