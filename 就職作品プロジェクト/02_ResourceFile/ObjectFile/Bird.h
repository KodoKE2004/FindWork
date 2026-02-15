#pragma once
#include "Square.h"
#include "Calculator.h"

class Bird : public Square
{
private:
    using MotionState = Calculator::Physics::MotionState;
    MotionState m_Motion;

    bool m_IsAlive  = true;
    bool m_IsActive = true;

public:
    Bird(Camera& cam);
    void Initialize() override;
    void Update()     override;
    void Draw()       override;
    void Finalize()   override;

    bool IsAlive()  const { return m_IsAlive; }
    bool IsActive() const { return m_IsActive;}
    
    void Alive() {
        m_IsAlive = true;
    }
    void DeAlive() {
        m_IsAlive = false;
    }

    void Active() {
        m_IsActive = true;
    }
    void DeActive() {
        m_IsActive = false;
    }    
    MotionState& GetMotionState() {
        return m_Motion;
    }
};

