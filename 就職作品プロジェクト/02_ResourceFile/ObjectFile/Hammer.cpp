#include "Hammer.h"
#include "Application.h"

Hammer::Hammer(Camera* cam) : Square(cam)
{
}

void Hammer::Initialize()
{
    Square::Initialize();
}

void Hammer::Update()
{
    if (Input::GetKeyPress(VK_A)) {
        m_Position.x -= 1.0f;
    }
    if (Input::GetKeyPress(VK_D)) {
        m_Position.x += 1.0f;
    }
    if (Input::GetKeyTrigger(VK_RETURN) && !m_isAttack) { 
        Attack(Application::GetDeltaTime());
    }
    if (m_isAttack) {

    }
}

void Hammer::Draw()
{
    Square::Draw();
}

void Hammer::Finalize()
{
    Square::Finalize();
}

void Hammer::Attack(float tick)
{   
    m_Position.y = - 200.0f;
    m_isAttack = true;
}