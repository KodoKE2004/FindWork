#include "Player.h"
#include "Car.h"
#include "Application.h"

#include <cmath>
#include <algorithm>

namespace
{
    float CalculateJumpInitialVelocity(float gravity, float jumpHeight)
    {
        const float safeGravity = max(gravity   , 0.0f);
        const float safeHeight  = max(jumpHeight, 0.0f);
        if (safeGravity <= 0.0f || safeHeight <= 0.0f)
        {
            return 0.0f;
        }

        return std::sqrt(2.0f * safeGravity * safeHeight);
    }

    PlayerTuningState CalculateDefaultTuningParameters()
    {
        PlayerTuningState params{};
        params.jumpHeight         = 400.0f;
        params.gravity            = 1800.0f;
        params.jumpInitialVelocity = CalculateJumpInitialVelocity(params.gravity, params.jumpHeight);
        params.groundMoveSpeed     = 12.0f;
        params.airControl.acceleration = 3600.0f;
        params.airControl.maxSpeed     = 700.0f;
        params.airControl.airFriction  = 3200.0f;
        return params;
    }
}

Player::Player(Camera* cam) : Square(cam)
{
    m_TuningParameters = CalculateDefaultTuningParameters();
    m_AirControl = m_TuningParameters.airControl;
    ApplyJumpState();
    ApplyAirControlState();
}

void Player::Initialize()
{
    Square::Initialize();
    SetPos(0.0f, -200.0f, 0.0f);
    m_VerticalMotion.groundY = m_Position.y;
    m_VerticalMotion.velocity = 0.0f;
    m_HorizontalMotion.velocity = 0.0f;
}

void Player::Update()
{
    const float deltaTime = Application::GetDeltaTime();
    int horizontalInput = 0;
    if (Input::GetKeyPress(VK_A)) {
        if (m_IsGround) 
            m_Position.x -= m_MoveSpeed;
        else
            horizontalInput -= 1.0f;
    }
    if (Input::GetKeyPress(VK_D)) {
        if (m_IsGround)
            m_Position.x += m_MoveSpeed;
        else
            horizontalInput += 1.0f;
    }
    
    if (m_IsGround && horizontalInput != 0)
    {
        m_Position.x += static_cast<float>(horizontalInput) * m_MoveSpeed;
    }

    const bool jumpTriggered = m_IsGround && Input::GetKeyTrigger(VK_SPACE);
    if (jumpTriggered)
    {
        m_VerticalMotion.velocity = m_JumpInitialVelocity;
        m_IsGround = false;
    }

    const float updatedY = Math::Physics::UpdateVerticalPosition(m_VerticalMotion, m_Position.y, deltaTime);
    m_Position.y = updatedY;

    if (std::fabs(m_Position.y - m_VerticalMotion.groundY) <= 0.001f) {
        m_IsGround = true;
        m_HorizontalMotion.velocity = 0.0f;
    }

}

void Player::Draw()
{
    Square::Draw();
}

void Player::Finalize()
{
    Square::Finalize();
}

void Player::SetAirControlState(const PlayerAieControlState& state)
{
    m_AirControl = state;
    m_TuningParameters.airControl = state;
    ApplyAirControlState();
}

void Player::ApplyAirControlState()
{
    m_HorizontalMotion.acceleration = m_AirControl.acceleration;
    m_HorizontalMotion.maxSpeed     = m_AirControl.maxSpeed;
    m_HorizontalMotion.airFriction  = m_AirControl.airFriction;
}

void Player::ApplyJumpState()
{
    m_JumpInitialVelocity = m_TuningParameters.jumpInitialVelocity;
    if (m_JumpInitialVelocity <= 0.0f)
    {
        m_JumpInitialVelocity = CalculateJumpInitialVelocity(
            m_TuningParameters.gravity,
            m_TuningParameters.jumpHeight);
    }

    m_VerticalMotion.gravity = m_TuningParameters.gravity;
    m_VerticalMotion.weight  = 1.0f;
    m_VerticalMotion.terminalVelocity = - max(m_TuningParameters.gravity, 0.0f) * 5.0f;
}

