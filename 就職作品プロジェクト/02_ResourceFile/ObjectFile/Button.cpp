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
    m_Text->SetScale( 51.2f, 43.8f, 1.0f);
}

void Button::Update()
{
    MouseObject::Update();
    m_Text->SetPos(GetPos());
}

void Button::Draw()
{
    MouseObject::Draw();
}

void Button::Finalize()
{
    m_Text->Finalize();
    m_Text = nullptr;
    MouseObject::Finalize();
}

void Button::SetTextTexture(std::shared_ptr<Texture> m_Tex)
{
    m_Text->SetTexture(m_Tex);
}

void Button::SetTextColor(const Color color)
{
    if (m_Text)
    {
        m_Text->SetColor(color);
    }
}

void Button::SetTextColor(const float r, const float g, const float b, const float a)
{
    if (m_Text)
    {
        m_Text->SetColor(r, g, b, a);
    }
}
