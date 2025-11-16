#pragma once
#include "Square.h"
#include "Camera.h"
#include "Collider.h"

struct PlayerAieControlState
{
    float acceleration = 4200.0f;   //
    float maxSpeed     = 650.0f ;   //
    float airFriction  = 2200.0f;   //
};


class Player : public Square
{
private:
    Math::Physics::VerticalMotionState   m_VerticalMotion;
    Math::Physics::HorizontalMotionState m_HorizontalMotion;
    PlayerAieControlState                m_AirControl;

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

    void SetAirControlState(const PlayerAieControlState& state);
    
private:
    void ApplyAirControlState();
};