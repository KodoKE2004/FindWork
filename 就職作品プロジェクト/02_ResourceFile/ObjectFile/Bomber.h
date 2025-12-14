#pragma once
#include "Square.h"
#include "BomTimeLimit.h"

class Bomber : public Square
{
private:

    Square* m_Rope   = nullptr;  
    Square* m_Number = nullptr;
    
    float m_FillRatio = 1.0f;
    int   m_Count     = 3;

    NVector3 m_BasePos{};
    NVector3 m_BaseScale{};

    bool     m_HasBase = false;
    bool     m_isReadyExpo = false;

public:
    Bomber(Camera& cam);
    ~Bomber() = default;
    void Initialize() override;
    void Update()     override;
    void Draw()       override;
    void Finalize()   override;

    void CountDown();

    void SetFillRatio(float ratio);
    float GetFillRatio() const { return m_FillRatio; }

    void ReadyExpo() { m_isReadyExpo = true; }
    bool IsReadyExpo() const { return m_isReadyExpo; }

    int GetCount() const {
        return m_Count;
    }
private:
    void UpdateUV();

    // Object‚Ì¶’[‚ğŠî€‚ÉTexture‚ÌUV‚É‡‚í‚¹‚Ä
    // Pos‚ÆScale‚ğ•â³
    void ApplyFillTransform();

    // 
    void CountDownTexture();
};

