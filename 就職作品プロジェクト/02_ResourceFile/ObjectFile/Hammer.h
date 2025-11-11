#pragma once
#include "Square.h"

class Hammer : public Square
{
private:
    bool  m_isAttack = false;

    float m_AttackFrame = 0.0f;
    float m_Duration    = 0.0f;
    NVector3 m_AttackPoint {} ;

public:
    Hammer(Camera* cam);
    ~Hammer() override = default ;
    void Initialize() override;
    void Update()     override;
    void Draw()       override;
    void Finalize()   override;

    void Attack(float tick);
};

