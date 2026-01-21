#include "MouseObject.h"

MouseObject::MouseObject(Camera& cam) : Square(cam)
{
}

void MouseObject::Initialize()
{
    Square::Initialize();
    m_isDrag    = false;
    m_isInside  = false;
}

void MouseObject::Update()
{
    const auto mousePos = Input::GetMousePos();
    const auto pos = GetPos();
    const auto scale = GetScale();
    const float halfWidth  = scale.x * 0.5f;
    const float halfHeight = scale.y * 0.5f;

    // オブジェクトの範囲内にマウスがあるか
    m_isInside = std::abs(mousePos.x - pos.x) <= halfWidth &&
                 std::abs(mousePos.y - pos.y) <= halfHeight;
    
    if (Input::GetMouseTrigger(vkLEFT))
    {
        if (m_isInside)
        {
            m_isDrag = true;
        }
    }
    if (Input::GetMouseRelease(vkLEFT))
    {
        m_isDrag = false;
    }
}

void MouseObject::Draw()
{
    Square::Draw();
}

void MouseObject::Finalize()
{
    Square::Finalize();
}

bool MouseObject::IsDrag() const
{
    return m_isDrag;
}

bool MouseObject::IsInside() const 
{
    return m_isInside;
}
