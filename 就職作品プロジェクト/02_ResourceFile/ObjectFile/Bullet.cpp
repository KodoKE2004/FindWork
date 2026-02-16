#include "Bullet.h"
#include "../Game.h"
#include <cmath>

using namespace DirectX::SimpleMath;

Bullet::Bullet(Camera& cam) : Square(cam), m_Direction(Vector3::Zero), m_Speed(10.0f), m_IsAlive(false)
{

}

void Bullet::Initialize() {
    Square::Initialize();

    m_Scale = NVector3(100.0f, 100.0f, 100.0f);
    m_Direction = Vector3(0.0f,1.0f,0.0f);
    m_Speed = 10.0f;
}

void Bullet::Shoot(const Vector3& pos, const Vector3& dir) {
    m_Position = NVector3(pos.x, pos.y, pos.z);
    m_Direction = dir;
    m_IsAlive = true;
}

void Bullet::Update() {
    if (!m_IsAlive) return;

    // ¡‰ñ‚Í’Pƒ‚Éã•ûŒü‚ÉˆÚ“®‚³‚¹‚é
    m_Position.y += m_Direction.y * m_Speed;

    if (m_Position.y > 700.0f) {
        m_IsAlive = false;
    }
}
void Bullet::Draw() {
    if (this == nullptr) return;
    if (!m_IsAlive)      return;
    Square::Draw();
}
void Bullet::Finalize() {
    Square::Finalize();
}