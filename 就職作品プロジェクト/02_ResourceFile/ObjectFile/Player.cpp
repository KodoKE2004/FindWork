#include "Player.h"

Player::Player(Camera* cam) : Square(cam)
{
}

void Player::Initialize()
{
    Square::Initialize();
    SetPos(0.0f, -200.0f, 0.0f);
}

void Player::Update()
{
    if (Input::GetKeyPress(VK_A)) {
        m_Position.x -= 1.0f;
    }
    if (Input::GetKeyPress(VK_D)) {
        m_Position.x += 1.0f;
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


