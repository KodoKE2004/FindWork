#include "DragController.h"
#include "input.h"
#include <DirectXMath.h>
#include <cmath>
DragController::DragController(Camera& cam) : MouseObject(cam)
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
    MouseObject::Update();
    if (!m_isDrag) { return; }
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
