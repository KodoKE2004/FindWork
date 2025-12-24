#include "Transform.h"

void Transform::SetTransform(const Transform _Transform)
{
    SetPos   (_Transform.GetPos());
    SetRotate(_Transform.GetRotate());
    SetScale (_Transform.GetScale());
}

void Transform::SetTransform(const NVector3 _Pos, const NVector3 _Rotate, const NVector3 _Scale)
{
    SetPos   (_Pos   );
    SetRotate(_Rotate);
    SetScale (_Scale );
}

void Transform::SetPos(const NVector3 _Pos)
{
    m_Position = _Pos;
}

void Transform::SetPos(const DirectX::SimpleMath::Vector3 _Pos)
{
    SetPos(_Pos.x, _Pos.y, _Pos.z);
}

void Transform::SetPos(const float _PosX, const float _PosY, const float _PosZ)
{
    m_Position.x = _PosX;
    m_Position.y = _PosY;
	m_Position.z = _PosZ;
}

void Transform::SetPos(const float _Pos)
{
    m_Position.x = _Pos;
    m_Position.y = _Pos;
	m_Position.z = _Pos;
}

void Transform::SetRotate(const NVector3 _Rotate)
{
	m_Rotation = _Rotate;
}

void Transform::SetRotate(const DirectX::SimpleMath::Vector3 _Rotate)
{
    SetRotate(_Rotate.x, _Rotate.y, _Rotate.z);
}

void Transform::SetRotate(const float _RotateX, const float _RotateY, const float _RotateZ)
{
    m_Rotation.x = _RotateX;
    m_Rotation.y = _RotateY;
	m_Rotation.z = _RotateZ;
}

void Transform::SetRotate(const float _Rotate)
{
    m_Rotation.x = _Rotate;
    m_Rotation.y = _Rotate;
	m_Rotation.z = _Rotate;
}

void Transform::SetScale(const NVector3 _Scale)
{
	m_Scale = _Scale;
}

void Transform::SetScale(const DirectX::SimpleMath::Vector3 _Scale)
{
    SetScale(_Scale.x, _Scale.y, _Scale.z);
}

void Transform::SetScale(const float _ScaleX, const float _ScaleY, const float _ScaleZ)
{
    m_Scale.x = _ScaleX;
    m_Scale.y = _ScaleY;
	m_Scale.z = _ScaleZ;
}

void Transform::SetScale(const float _Scale)
{
    m_Scale.x = _Scale;
    m_Scale.y = _Scale;
	m_Scale.z = _Scale;
}

Transform Transform::GetTransform() const
{
    Transform result;

    result.SetPos(GetPos());
    result.SetRotate(GetRotate());
    result.SetScale(GetScale());

    return result;
}
