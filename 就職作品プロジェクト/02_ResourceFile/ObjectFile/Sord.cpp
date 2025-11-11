#include "Sord.h"

Sord::Sord(Camera* cam) : Square(cam)
{
}

void Sord::Initialize()
{
    Square::Initialize();
}

void Sord::Update()
{
    if (Input::GetKeyPress(VK_W)) {
        m_Position.y += 2.0f;
    }
    if (Input::GetKeyPress(VK_A)) {
        m_Position.x -= 2.0f;
    }
    if (Input::GetKeyPress(VK_S)) {
        m_Position.y -= 2.0f;
    }
    if (Input::GetKeyPress(VK_D)) {
        m_Position.x += 2.0f;
    }
}

void Sord::Draw()
{
    Square::Draw();
}

void Sord::Finalize()
{   
    
}

void Sord::Attack()
{   

}


