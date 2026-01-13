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
    m_Text->SetScale(150.0f, 50.0f, 1.0f);
    instance.GetCurrentScene()->GetSceneObjects().emplace_back(m_Text);
}

void Button::Update()
{
    MouseObject::Update();
    m_Text->SetPos(GetPos().x, GetPos().y + 5.0f, GetPos().z);
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
