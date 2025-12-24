#include "Theme.h"
#include "Game.h"
#include "Application.h"

Theme::Theme(Camera& cam):Square(cam)
{
}

void Theme::Initialize()
{
    auto& instance = Game::GetInstance();
    TextureManager* textureManager = instance;

    Square::Initialize();

    SetShader("VS_Alpha","PS_Alpha");
    SetTexture(textureManager->GetTexture("Plane.png"));

}

void Theme::Update()
{
}

void Theme::Draw()
{
    Square::Draw();
}

void Theme::Finalize()
{
    Square::Finalize();
}
