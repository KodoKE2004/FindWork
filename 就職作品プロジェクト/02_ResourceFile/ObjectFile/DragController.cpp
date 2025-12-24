#include "DragController.h"
#include "input.h"
#include <DirectXMath.h>
#include <cmath>
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

        m_IsDragging = isOverObject;
    }

    if (Input::GetMouseRelease(vkLEFT))
    {
        m_IsDragging = false;
    }

    if (!m_IsDragging)
    {
        m_DragOffset = {};
        return;
    }

    if (m_MoveDir != MOVE_NONE)
    {
        m_Rotation.z = MOVE_ANGLE[m_MoveDir];
    }

    const DirectX::SimpleMath::Vector2 mouseDelta = Input::GetMouseDelta();

    const float angleDeg = m_Rotation.z;
    const float angleRad = DirectX::XMConvertToRadians(angleDeg);
    const float dirX = std::cos(angleRad);
    const float dirY = std::sin(angleRad);

    const float axisOffset = mouseDelta.x * dirX + mouseDelta.y * dirY;
    m_DragOffset.x = dirX * axisOffset;
    m_DragOffset.y = dirY * axisOffset;

    auto pos = GetPos();
    pos.x += m_DragOffset.x;
    pos.y += m_DragOffset.y;

    SetPos(pos);
}

void DragController::Draw()
{
    Square::Draw();
}

void DragController::Finalize()
{
    Square::Finalize();
}
