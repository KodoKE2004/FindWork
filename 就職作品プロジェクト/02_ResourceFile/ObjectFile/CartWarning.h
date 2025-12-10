#pragma once
#include "Square.h"
class CartWarning : public Square
{
private:
    bool m_isVisible = false;
    float m_PulseTimer     = 0.0f;
    float m_PulseAmplitude = 0.15f;
    float m_PulseSpeed     = 6.0f;
    float m_TowardsCenterOffset = 150.0f;
    NVector3 m_AdditionalOffset = NVector3(0.0f, 0.0f, 0.0f);
    NVector3 m_DefaultScale = NVector3(220.0f, 220.0f, 1.0f);
    NVector3 m_BasePosition = NVector3(0.0f, 0.0f, 0.0f);

public:
    CartWarning(Camera& cam);
    ~CartWarning() = default;
    void Initialize() override;
    void Update()     override;
    void Draw()       override;
    void Finalize()   override;

    void Activate(const NVector3& startPos);
    void Deactivate();

    void SetBaseScale(const NVector3& scale);
    void SetAdditionalOffset(const NVector3& offset) { m_AdditionalOffset = offset; }
    void SetTowardsCenterOffset(float offset) { m_TowardsCenterOffset = offset; }
    bool IsVisible() const { return m_isVisible; }
};

