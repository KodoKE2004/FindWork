#pragma once
#include "Square.h"
#include "Calculator.h"
class Stone : public Square
{
private:
    bool m_isActive;
    Math::Physics::VerticalMotionState m_VerticalMotion{};

public:
    Stone(Camera& cam);
    void Initialize() override;
    void Update()     override;
    void Draw()       override;
    void Finalize()   override;
    
    bool IsActive() const {
        return m_isActive;
    }
};


