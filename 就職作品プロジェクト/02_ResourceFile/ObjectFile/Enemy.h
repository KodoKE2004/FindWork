#pragma once
#include "Square.h"
class Enemy : public Square
{
private:
    bool m_isDeath = false;
public:
    Enemy(Camera* cam);
    ~Enemy() = default ;
    void Initialize() override;
    void Update()     override;
    void Draw()       override;
    void Finalize()   override;

    void Death() ;
    bool IsDeath() {
        return m_isDeath;
    }
};

