#include "Player.h"
#include "Car.h"
#include "Application.h"

#include <cmath>

Player::Player(Camera* cam) : Square(cam)
{
    m_VerticalMotion.gravity = 980.0f;
    m_VerticalMotion.weight  = 1.2f;
    m_VerticalMotion.terminalVelocity = -3000.0f;
}

void Player::Initialize()
{
    Square::Initialize();
    SetPos(0.0f, -200.0f, 0.0f);
    m_VerticalMotion.groundY = m_Position.y;
    m_VerticalMotion.velocity = 0.0f;
}

void Player::Update()
{
    const float deltaTime = Application::GetDeltaTime();
    int horizontalInput = 0;
    if (Input::GetKeyPress(VK_A)) {
        horizontalInput -= 1;
        if (m_IsGround)
        {
            m_Position.x -= m_MoveSpeed;
        }
    }
    if (Input::GetKeyPress(VK_D)) {
        horizontalInput += 1;
        if (m_IsGround)
        {
            m_Position.x += m_MoveSpeed;
        }
    }

    const bool jumpTriggered = m_IsGround && Input::GetKeyTrigger(VK_SPACE);
    if (jumpTriggered)
    {
        m_VerticalMotion.velocity = JumpValue;
        m_IsGround = false;
        Math::Physics::InitializeHorizontalVelocity(m_HorizontalMotion, m_Position.x, m_MoveSpeed, deltaTime);
    }

    if (!m_IsGround)
    {
        m_Position.x = Math::Physics::UpdateHorizontalPosition(
            m_HorizontalMotion,
            m_Position.x,
            deltaTime,
            horizontalInput
        );

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


