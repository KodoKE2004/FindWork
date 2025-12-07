#pragma once
#include "Square.h"
class Bomber : public Square
{
private:
    float m_FillRatio = 1.0f;

    NVector3 m_BasePos{};
    NVector3 m_BaseScale{};
    bool     m_HasBase = false;

public:
    Bomber(Camera* cam);
    ~Bomber() = default;
    void Initialize() override;
    void Update()     override;
    void Draw()       override;
    void Finalize()   override;

    void SetFillRatio(float ratio);
    float GetFillRatio() const { return m_FillRatio; }

private:
    void UpdateUV();
    void ApplyFillTransform();
};

