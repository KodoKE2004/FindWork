#pragma once
#include "Square.h"
#include "Calculator.h"
class Stone : public Square
{
private:
    bool m_isActive;
    Calculator::Physics::VerticalMotionState m_VerticalMotion{};

public:
    Stone(Camera& cam);
    void Initialize() override;
    void Update()     override;
    void Draw()       override;
    void DrawInstanced(const std::vector<std::shared_ptr<Stone>>& stones);
    void Finalize()   override;
    
    bool IsActive() const {
        return m_isActive;
    }
};


