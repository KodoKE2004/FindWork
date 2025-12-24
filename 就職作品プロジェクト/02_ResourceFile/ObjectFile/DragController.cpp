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
    const float delta = mouseDelta.x;

    m_DragOffset.x = m_Dir.x * delta;
    m_DragOffset.y = m_Dir.y * delta;

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
