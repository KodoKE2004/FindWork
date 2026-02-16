#include "DragController.h"
#include "input.h"
#include <DirectXMath.h>
#include <cmath>
#include <algorithm>

DragController::DragController(Camera& cam) : MouseObject(cam)
{
}

DragController::~DragController()
{
}

void DragController::Initialize()
{   
    Square::Initialize();
    m_isLimit = false;
    m_LimitRange = { 0.0f, 0.0f, 0.0f };
}

void DragController::Update()
{
    MouseObject::Update();
    if (!m_isDrag) { return; }

    if (m_isDrag && !m_IsDragging)
    {
        m_MouseDownPos = Input::GetMousePos();

        const auto objectPos = GetPos();
        m_ObjectDownPos = { objectPos.x, objectPos.y };
        m_IsDragging = true;
    }

    if (!m_isDrag)
    {
        m_IsDragging = false;
        return;
    }
    if (m_MoveDir == MOVE_NONE){ return; }

    m_Rotation.z = MOVE_ANGLE[m_MoveDir];
    
    const DirectX::SimpleMath::Vector2 currentMousePos = Input::GetMousePos();
    const DirectX::SimpleMath::Vector2 delta = currentMousePos - m_MouseDownPos;
    const float rad = DirectX::XMConvertToRadians(MOVE_ANGLE[m_MoveDir]);
    DirectX::SimpleMath::Vector2 dir(std::cos(rad), std::sin(rad));

    if (dir.LengthSquared() > 0.0f){
        dir.Normalize();
    }

    const float t = (delta.x * dir.x) + (delta.y * dir.y);
    Vector2 pos = m_ObjectDownPos + (dir * t);

    if (m_isLimit)
    {    
        std::clamp(pos.x, - m_LimitRange.x, m_LimitRange.x);
        std::clamp(pos.y, - m_LimitRange.y, m_LimitRange.y);
    }

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
