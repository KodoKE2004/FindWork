#pragma once
#include "MouseObject.h"
class Button : public MouseObject
{
private:
    std::shared_ptr<Square> m_Text;
    NVector3 m_BaseScale;
public:
    Button(Camera& cam);
    void Initialize() override;
    void Update()     override;
    void Draw()       override;
    void Finalize()   override;

    void SetBaseScale(NVector3 scale) {
        m_BaseScale = scale;
    }
    void SetTextScale(NVector3 scale) {
        m_Text->SetScale(scale);
    }
    void SetTextTexture(std::shared_ptr<Texture> m_Tex);
};

