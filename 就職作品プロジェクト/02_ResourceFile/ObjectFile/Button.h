#pragma once
#include "MouseObject.h"
class Button : public MouseObject
{
private:
    pShared<Square> m_Text;
    NVector3 m_BaseScale;
public:
    Button(Camera& cam);
    void Initialize() override;
    void Update()     override;
    void Draw()       override;
    void Finalize()   override;

    void SetBaseScale(NVector3 scale) {
        m_BaseScale = scale;
        SetScale(scale);
    }
    void SetTextScale(NVector3 scale) {
        m_Text->SetScale(scale);
    }
    void SetTextTexture(pShared<Texture> m_Tex);

    void SetTextColor(const Color color);
    void SetTextColor(const float r, const float g, const float b, const float a);

    NVector3 GetBaseScale() const {
        return m_BaseScale;
    }

    pShared<Square> GetTextObject() const {
        return m_Text;
    }
};

