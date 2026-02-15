#include "Bird.h"
#include "Game.h"
#include "Application.h"
#include <algorithm>
#include <cmath>

namespace
{
    Calculator::Physics::MotionState CreateMotionState()
    {
        Calculator::Physics::MotionState state{};
        state.velocity = Vector2::Zero;
        state.mass = 1.0f;
        state.mag = 120.0f;
        state.enableGravity = true;
        state.integrateX = true;
        state.integrateY = true;
        state.groundY = -700.0f;
        state.clampToGround = true;
        return state;
    }

    constexpr float kMoveAccel = 2400.0f;
    constexpr float kMaxSpeedX = 600.0f;
    constexpr float kNoInputDamping = 8.0f;
}
Bird::Bird(Camera& cam) : Square(cam)
{
}

void Bird::Initialize()
{
    Square::Initialize();
    TextureManager* textureMgr = Game::GetInstance();
    
    m_Motion = CreateMotionState();
    SetTexture(textureMgr->GetTexture("GameScene/Sparrow.png"));

    SetPos  (   0.0f,   0.0f, 0.0f);
    SetScale( 100.0f, 100.0f, 1.0f);

}

void Bird::Update()
{
    if (IsAlive())
    {
        Vector2 pos = Input::GetMousePos();
        const float WIDTH = static_cast<float>(Application::GetWidth());
        const float HEIGHT = static_cast<float>(Application::GetHeight());

        const float WIDTH_HALF = WIDTH * 0.5f;
        const float HEIGHT_HALF = HEIGHT * 0.5f;
        bool isScreenOut = (pos.x < - WIDTH_HALF  || pos.x > WIDTH_HALF ) ||
                           (pos.y < - HEIGHT_HALF || pos.y > HEIGHT_HALF);
        if (!isScreenOut)
        { 
            SetPos(pos.x,pos.y,0.0f);
        }
    }
    else
    {
        const float dt = Application::GetDeltaTime();
        if (dt <= 0.0f)
        {
            return;
        }

        float inputX = 0.0f;
        if (Input::GetKeyPress(VK_A) || Input::GetKeyPress(VK_LEFT))
        {
            inputX -= 1.0f;
        }
        if (Input::GetKeyPress(VK_D) || Input::GetKeyPress(VK_RIGHT))
        {
            inputX += 1.0f;
        }

        m_Motion.velocity.x += inputX * kMoveAccel * dt;
        m_Motion.velocity.x = std::clamp(m_Motion.velocity.x, -kMaxSpeedX, kMaxSpeedX);

        if (std::abs(inputX) < 0.001f)
        {
            const float damping = std::clamp(kNoInputDamping * dt, 0.0f, 1.0f);
            m_Motion.velocity.x -= m_Motion.velocity.x * damping;
        }

        const NVector3 currentPos = GetPos();
        const Vector2 nextPos = Calculator::Physics::StepRigidBody(m_Motion, Vector2(currentPos.x, currentPos.y), dt);
        SetPos(nextPos.x, nextPos.y, currentPos.z);
    }
}

void Bird::Draw()
{
    Square::Draw();
}

void Bird::Finalize()
{
    Square::Finalize();
}
