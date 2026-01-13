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
    instance.GetCurrentScene()->GetSceneObjects().emplace_back(m_Text);
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
    MouseObject::Finalize();
}

void Button::SetTextTexture(std::shared_ptr<Texture> m_Tex)
{
    m_Text->SetTexture(m_Tex);
}
