#include "BomTimeLimit.h"
#include "Game.h"

BomTimeLimit::BomTimeLimit(Camera* cam) : Square(cam)
{
}

void BomTimeLimit::Initialize()
{
    Square::Initialize();
    m_Count = 3;
    SetPos( -565.0f,-240.0f,0.0f);
    SetScale(80.0f, 80.0f, 1.0f);
}

void BomTimeLimit::Update()
{
}

void BomTimeLimit::Draw()
{
    Square::Draw();
}

void BomTimeLimit::Finalize()
{
    Square::Finalize();
}

void BomTimeLimit::ChangeTexture()
{
    auto& instance = Game::GetInstance();
    TextureManager* textureMgr = instance;

    switch (m_Count)
    {
    case 3: SetTexture(textureMgr->GetTexture("CountThree.png")); break;
    case 2: SetTexture(textureMgr->GetTexture("CountTwo.png"));   break;
    case 1: SetTexture(textureMgr->GetTexture("CountOne.png"));   break;
    default: SetPos(1000.0f,0.0f,0.0f); break;
    }
}
