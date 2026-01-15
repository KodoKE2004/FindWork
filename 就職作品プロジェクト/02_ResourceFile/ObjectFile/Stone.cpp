#include "Stone.h"
#include "Game.h"

Stone::Stone(Camera& cam) : Square(cam)
{
}

void Stone::Initialize()
{
    Square::Initialize();
    auto& instance = Game::GetInstance();
    TextureManager* textureMgr = instance;
    SetTexture(textureMgr->GetTexture("Stone.png"));

    m_isActive = true;

    m_VerticalMotion.velocity = 0.0f;
    m_VerticalMotion.gravity = 1800.0f;
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
