#pragma once
#include "Square.h"
#include "Calculator.h"
class Stone : public Square
{
private:
    using Motion = Calculator::Physics::MotionState;
    bool m_isActive;
    Motion m_Motion{};

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

    void SetMotionState(Motion state) {
        m_Motion = state;
    }
};


