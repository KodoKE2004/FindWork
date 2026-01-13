#include "Button.h"
#include "Game.h"

Button::Button(Camera& cam) : MouseObject(cam)
{
}

void Button::Initialize()
{
    MouseObject::Initialize();
    auto& instance = Game::GetInstance();
    TextureManager* textureMgr = instance;

    m_Text = instance.AddObject<Square>();
    m_Text->SetName("m_Text");
}

void Button::Update()
{
    MouseObject::Update();
}

void Button::Draw()
{
    MouseObject::Draw();
}

void Button::Finalize()
{
    MouseObject::Finalize();
}

void Button::SetTextTexture(std::shared_ptr<Texture> m_Tex)
{
    m_Text->SetTexture(m_Tex);
}
