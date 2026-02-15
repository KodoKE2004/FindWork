#include "Bird.h"
#include "Game.h"
#include "Application.h"
namespace
{
    Calculator::Physics::MotionState CreateMotionState()
    {
        Calculator::Physics::MotionState state{};
        state.velocity = NVector3(0.0f, 0.0f, 0.0f);
        state.constantAcceleration = NVector3(0.0f, -980.0f, 0.0f);
        state.terminalVelocity = NVector3(1200.0f, 9000.0f, 0.0f);
        state.mass = 1.0f;
        state.groundY = -700.0f;
        state.useGroundClamp = true;
        return state;
    }

    const NVector3 kBirdForce = {
        -10.0f, 5.0f, 0.0f
    };
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
