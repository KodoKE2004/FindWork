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
    if (Input::GetKeyPress(VK_A)) {
        m_Position.x -= m_MoveSpeed;
    }
    if (Input::GetKeyPress(VK_D)) {
        m_Position.x += m_MoveSpeed;
    }
    if (m_IsGround && Input::GetKeyTrigger(VK_SPACE))
    {
        m_VerticalMotion.velocity = JumpValue;
        m_IsGround = false;
    }

    const float updatedY = Math::Physics::UpdateVerticalPosition(m_VerticalMotion, m_Position.y, deltaTime);
    m_Position.y = updatedY;

    if (std::fabs(m_Position.y - m_VerticalMotion.groundY) <= 0.001f) {
        m_IsGround = true;
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


