#include "Application.h"
#include "Game.h"
#include "Math.h"
#include "Theme.h"

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
    if(!m_isActive){ 
        return; 
    }

    float elapsed = 



}

void Theme::Draw()
{
    if(!m_isActive){ return; }
    Square::Draw();
}

void Theme::Finalize()
{
    Square::Finalize();
}

void Theme::SetActive()
{   
    m_isActive = true;
}

void Theme::SetActive(const bool isActive)
{
    m_isActive = isActive;
}
