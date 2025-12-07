#include "Bomber.h"
#include "Game.h"

Bomber::Bomber(Camera* cam) : Square(cam)
{
}

void Bomber::Initialize()
{
    Square::Initialize();

    TextureManager* textureMgr = Game::GetInstance();

    SetTexture(textureMgr->GetTexture("Bomber.png"));
    SetPos(NVector3(-100.0f, -250.0f, 0.0f));
    SetScale(NVector3(700.0f, 100.0f, 1.0f));
    m_NumU = 7;
    m_NumV = 1;
    m_SplitX = 7;
    m_SplitY = 1;
    SetShader("VS_Alpha", "PS_Alpha");

}

void Bomber::Update()
{
}

void Bomber::Draw()
{
    Square::Draw();
}

void Bomber::Finalize()
{
    Square::Finalize();
}

