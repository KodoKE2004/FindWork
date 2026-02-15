#include "Player.h"
#include "Car.h"
#include "Application.h"
#include "Game.h"

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
}

Player::Player(Camera& cam) : Square(cam)
{

}

void Player::Initialize()
{
    Square::Initialize();
    SetPos(0.0f, -200.0f, 0.0f);

    auto& instance = Game::GetInstance();
    
}

void Player::Update()
{
    const float tick = Application::GetDeltaTime();
    
    

}

void Player::Draw()
{
    Square::Draw();

}

void Player::Finalize()
{
    Square::Finalize();
}

