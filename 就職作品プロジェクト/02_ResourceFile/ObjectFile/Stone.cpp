#include "Stone.h"
#include "Game.h"
#include "Calculator.h"
#include <random>

Stone::Stone(Camera& cam) : Square(cam)
{
}

namespace 
{
    void SpawnPos(Stone& stone)
    {
        static std::mt19937 engine{ std::random_device{}() };
        std::uniform_real_distribution<float> distX(-400.0f, 400.0f);
        std::uniform_real_distribution<float> distY(600.0f, 800.0f);
        float posX = distX(engine);
        float posY = distY(engine);
        stone.SetPos(posX, posY, 0.0f);
    }
}

void Stone::Initialize()
{
    Square::Initialize();
    auto& instance = Game::GetInstance();
    TextureManager* textureMgr = instance;
    SetTexture(textureMgr->GetTexture("Stone.png"));
    SpawnPos(*this);

    m_isActive = true;

    m_VerticalMotion.velocity = 0.0f;
    m_VerticalMotion.gravity = 1200.0f;
    m_VerticalMotion.weight = 1.0f;
    m_VerticalMotion.terminalVelocity = -9000.0f;
    m_VerticalMotion.groundY = -10000.0f;

}

void Stone::Update()
{
    if (!IsActive()) {
        return;
    }

    const float deltaTime = Application::GetDeltaTime();
    m_Position.y = Math::Physics::UpdateVerticalPosition(m_VerticalMotion, m_Position.y, deltaTime);
}

void Stone::Draw()
{
    if (!IsActive()) {
        return;
    }
    Square::Draw();
}

void Stone::Finalize()
{
    Square::Finalize();
}
