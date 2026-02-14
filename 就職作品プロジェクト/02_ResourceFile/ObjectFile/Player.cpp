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
    m_Bullet = std::make_shared<Bullet>(instance.GetCamera());
    m_Bullet->Initialize();
    
}

void Player::Update()
{
    const float tick = Application::GetDeltaTime();
    m_CreateBulletElapsed += tick;
    if (m_CreateBulletTime <= m_CreateBulletTime)
    {
        m_CreateBulletElapsed = 0.0f;
        auto& instance = Game::GetInstance();
        TextureManager* textureMgr = instance;

        auto bullet = std::make_shared<Bullet>(instance.GetCamera());

        NVector3 pos     = GetPos();
        float scaleHalfY = GetScale().y * 0.5f;
        float baseScale = 100.0f;
        DirectX::SimpleMath::Vector2 scaleBullet{
            12.0f   * baseScale,
            13.717f * baseScale
        };
        
        bullet->Initialize();
        bullet->SetTexture(textureMgr->GetTexture("Bullet.png"));
        bullet->SetPos  (pos.x, pos.y + scaleHalfY, 0.0f);
        bullet->SetScale(scaleBullet.x, scaleBullet.y, 0.0f);
        bullet->Alive();
        m_BulletList.emplace_back(bullet);
    }
    
    for (auto bullet : m_BulletList)
    {
        bullet->Update();
    }

    m_BulletList.erase(
        std::remove_if(m_BulletList.begin(), m_BulletList.end(),
            [](const pShared<Bullet>& bullet)
            {
                return !bullet || !bullet->IsAlive();
            }),
        m_BulletList.end()
    );

}

void Player::Draw()
{
    Square::Draw();

}

void Player::Finalize()
{
    Square::Finalize();
}

