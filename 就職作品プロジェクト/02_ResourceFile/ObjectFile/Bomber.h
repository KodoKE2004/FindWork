#pragma once
#include "Square.h"

class Bomber : public Square
{
private:

    pShared<Square> m_Rope   = nullptr;
    pShared<Square> m_Number = nullptr;
    
    float m_FillRatio       = 1.0f; // UÇÃî‰ó¶(0.0f ~ 1.0f)

    NVector3 m_BasePos{};
    NVector3 m_BaseScale{};
    float    m_BaseRopeU = 1.0f;
    float    m_FillBomberU = 0.0311f;

    float    m_BaseLeftX = 0.0f;
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
    
    // ÉQÅ[ÉÄSceneÇÃBeató Ç…âûÇ∂ÇƒRopeÇÃç≈ëÂÇÃí∑Ç≥Çí≤êÆ
    void AdjustScaleByBeatTotal(int beatTotal, int maxBeat = 16);

    void ReadyExpo() { m_isReadyExpo = true; }
    bool IsReadyExpo() const { return m_isReadyExpo; }

    pShared<Square> GetRope() {
        if (m_Rope) {
            return m_Rope;
        }
        return nullptr; 
    }
    
    pShared<Square> GetNumber() {
        if (m_Number) {
            return m_Number;
        }
        return nullptr; 
    }

    float GetBomberU() const {
        if (!m_HasBase || m_FillBomberU <= 0.0f) {
            return 0.0f;
        }
        return m_FillBomberU;
    }

private:
    void UpdateUV();

    // ObjectÇÃç∂í[ÇäÓèÄÇ…TextureÇÃUVÇ…çáÇÌÇπÇƒ
    // PosÇ∆ScaleÇï‚ê≥
    void ApplyFillTransform();

    // 
    void CountDownTexture();

};

