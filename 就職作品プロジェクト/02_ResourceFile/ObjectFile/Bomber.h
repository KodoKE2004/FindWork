#pragma once
#include "Square.h"

class Bomber : public Square
{
private:

    Square* m_Rope   = nullptr;  
    Square* m_Number = nullptr;
    
    float m_FillRatio = 1.0f;  // U‚Ì”ä—¦(0.0f ~ 1.0f)

    NVector3 m_BasePos{};
    NVector3 m_BaseScale{};

    bool     m_HasBase = false;
    bool     m_isReadyExpo = false;

    int m_Count = 0;

public:
    Bomber(Camera& cam);
    ~Bomber() = default;
    void Initialize() override;
    void Update()     override;
    void Draw()       override;
    void Finalize()   override;

    void CountDown();
    void SetCount(const int count) { m_Count = count; }
    void  SetFillRatio(float ratio);
    float GetFillRatio() const { return m_FillRatio; }

    void ReadyExpo() { m_isReadyExpo = true; }
    bool IsReadyExpo() const { return m_isReadyExpo; }

private:
    void UpdateUV();

    // Object‚Ì¶’[‚ğŠî€‚ÉTexture‚ÌUV‚É‡‚í‚¹‚Ä
    // Pos‚ÆScale‚ğ•â³
    void ApplyFillTransform();

    // 
    void CountDownTexture();
};

