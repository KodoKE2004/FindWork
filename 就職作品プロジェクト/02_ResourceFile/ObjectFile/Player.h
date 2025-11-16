#pragma once
#include "Square.h"
#include "Camera.h"
#include "Collider.h"

class Player : public Square
{
private:
    Math::Physics::VerticalMotionState m_VerticalMotion;
    const float JumpValue = 850.0f;
    float m_MoveSpeed = 1.0f;
    bool m_IsGround = true;

public:
    Player(Camera* cam);
    ~Player() override = default;

    void Initialize() override;
    void Update()     override;
    void Draw()       override;
    void Finalize()   override;

    
};