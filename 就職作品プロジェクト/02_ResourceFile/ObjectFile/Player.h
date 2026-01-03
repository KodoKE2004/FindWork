#pragma once
#include "Camera.h"
#include "Math.h"
#include "Square.h"

struct PlayerAieControlState
{
    float acceleration = 4200.0f;   //
    float maxSpeed     = 650.0f ;   //
    float airFriction  = 100.0f;   //
};

struct PlayerTuningState
{
    float jumpHeight = 850.0f;          // ジャンプ力
    float gravity    = 1800.0f;         // 重力加速度
    float jumpInitialVelocity = 0.0f;   // 計算済みのジャンプ初速
    float groundMoveSpeed     = 12.0f;  // 地上での移動速度
    PlayerAieControlState airControl;   // 空中制御
};

class Player : public Square
{
private:
    Math::Physics::VerticalMotionState   m_VerticalMotion;
    Math::Physics::HorizontalMotionState m_HorizontalMotion;
    PlayerTuningState                    m_TuningParameters;
    PlayerAieControlState                m_AirControl;

    float m_JumpInitialVelocity = 0.0f;
    float m_MoveSpeed = 1.0f;
    bool  m_IsGround = true;

public:
    Player(Camera & cam);
    ~Player() override = default;

    void Initialize() override;
    void Update()     override;
    void Draw()       override;
    void Finalize()   override;

    void SetAirControlState(const PlayerAieControlState& state);
    
private:
    void ApplyAirControlState();
    void ApplyJumpState();
};