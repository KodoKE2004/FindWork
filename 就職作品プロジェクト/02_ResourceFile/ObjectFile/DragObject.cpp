#include "DragObject.h"

DragObject::DragObject(Camera& cam) : Square(cam)
{
}

void DragObject::Initialize()
{
    Square::Initialize();
    m_isDrag    = false;
    m_isInside  = false;
}

void DragObject::Update()
{
    const auto mousePos = Input::GetMousePos();
    const auto pos = GetPos();
    const auto scale = GetScale();
    const float halfWidth  = scale.x * 0.5f;
    const float halfHeight = scale.y * 0.5f;

    // オブジェクトの範囲内にマウスがあるか
    m_isInside = std::abs(mousePos.x - pos.x) <= halfWidth &&
                 std::abs(mousePos.y - pos.y) <= halfHeight;
    
    if (Input::GetMouseTrriger(vkLEFT))
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

void DragObject::Draw()
{
    Square::Draw();
}

void DragObject::Finalize()
{
    Square::Finalize();
}

bool DragObject::IsDrag()
{
    return m_isDrag;
}
