#include "DragController.h"
#include "input.h"
#include <cmath>
#include <DirectXMath.h>

DragController::DragController(Camera& cam) : Square(cam)
{
}

DragController::~DragController()
{
}

void DragController::Initialize()
{   
    Square::Initialize();
}

void DragController::Update()
{
    if (Input::GetMouseTrriger(vkLEFT))
    {
        const auto mousePos = Input::GetMousePos();
        const auto pos   = GetPos();
        const auto scale = GetScale();
        const float halfWidth  = scale.x * 0.5f;
        const float halfHeight = scale.y * 0.5f;

        // オブジェクトの範囲内にマウスがあるか
        const bool  isOverObject = std::abs(mousePos.x - pos.x) <= halfWidth &&
                                   std::abs(mousePos.y - pos.y) <= halfHeight;

        if (isOverObject)
        {
            m_IsDragging = true;
            m_MouseDownPos = mousePos;
            m_ObjectDownPos = DirectX::SimpleMath::Vector2(pos.x,pos.y);
        }
    }

    if (Input::GetMouseRelease(vkLEFT))
    {
        m_IsDragging = false;
    }

    if (!m_IsDragging)
    {
        return;
    }

    if (m_MoveDir == MOVE_NONE)
    {
        return;
    }

    m_Rotation.z = MOVE_ANGLE[m_MoveDir];
    
    const DirectX::SimpleMath::Vector2 currentMousePos = Input::GetMousePos();
    const DirectX::SimpleMath::Vector2 delta = currentMousePos - m_MouseDownPos;
    const float rad = DirectX::XMConvertToRadians(MOVE_ANGLE[m_MoveDir]);
    DirectX::SimpleMath::Vector2 dir(std::cos(rad), std::sin(rad));
    if (dir.LengthSquared() > 0.0f)
    {
        dir.Normalize();
    }

    const float t = (delta.x * dir.x) + (delta.y * dir.y);
    const auto pos = m_ObjectDownPos + (dir * t);

    SetPos(pos.x,pos.y, 0.0f);
}

void DragController::Draw()
{
    Square::Draw();
}

void DragController::Finalize()
{
    Square::Finalize();
}
