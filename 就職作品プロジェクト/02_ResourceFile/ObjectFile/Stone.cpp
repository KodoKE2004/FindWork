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
}

void Stone::Update()
{
    if (!IsActive()) {
        return;
    }
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
