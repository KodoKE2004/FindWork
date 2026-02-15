#pragma once
#include "Square.h"
#include "Calculator.h"
class Stone : public Square
{
private:
    using MotionState = Calculator::Physics::MotionState;
    bool m_isActive;
    MotionState m_Motion{};

public:
    Stone(Camera& cam);
    void Initialize() override;
    void Update()     override;
    void Draw()       override;
    void DrawInstanced(const vector<pShared<Stone>>& stones);
    void Finalize()   override;
    
    bool IsActive() const {
        return m_isActive;
    }

    void Active() {
        m_isActive = true;
    }
    void DeActive() {
        m_isActive = false;
    }

    void SetMotionState(MotionState state) {
        m_Motion = state;
    }
};


