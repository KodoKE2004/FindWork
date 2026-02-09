#pragma once
#include "Circle.h"

class Bird : public Circle
{
private:
    bool m_IsActive = true;
public:
    Bird(Camera& cam);
    void Initialize() override;
    void Update()     override;
    void Draw()       override;
    void Finalize()   override;

    bool IsActive() const { return m_IsActive; }
};

