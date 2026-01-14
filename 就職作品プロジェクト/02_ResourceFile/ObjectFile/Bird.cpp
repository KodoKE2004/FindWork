#include "Bird.h"
#include "Game.h"
#include "Application.h"
namespace
{
    
}

Bird::Bird(Camera& cam) : Square(cam)
{
}

void Bird::Initialize()
{
    Square::Initialize();
    auto& instance = Game::GetInstance();
    TextureManager* textureMgr = instance;
    SetTexture(textureMgr->GetTexture("Sparrow.png"));

    SetPos  (   0.0f,   0.0f, 0.0f);
    SetScale( 100.0f, 100.0f, 1.0f);
}

void Bird::Update()
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

void Bird::Draw()
{
    Square::Draw();
}

void Bird::Finalize()
{
    Square::Finalize();
}
