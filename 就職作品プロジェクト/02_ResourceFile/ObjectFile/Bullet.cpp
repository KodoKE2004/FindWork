#include "Bullet.h"
#include "../Game.h"
#include <cmath>

using namespace DirectX::SimpleMath;

Bullet::Bullet(Camera& cam) : Square(cam), m_Direction(Vector3::Zero), m_Speed(10.0f), m_IsAlive(false)
{

}

void Bullet::Initialize() {
    TextureManager* textureMgr = Game::GetInstance();
    Square::Initialize();
    
    auto placeHolder = textureMgr->GetTexture("placeholder.png");
    if(placeHolder){
        SetTexture(placeHolder);
    }

    m_Scale = NVector3(100.0f, 100.0f, 100.0f);
}

void Bullet::Shoot(const Vector3& pos, const Vector3& dir) {
    m_Position = NVector3(pos.x, pos.y, pos.z);
    m_Direction = dir;
    m_Direction.Normalize();
    m_IsAlive = true;
}

void Bullet::Update() {
    if (!m_IsAlive) return;

    m_Position.x += m_Direction.x * m_Speed;
    m_Position.y += m_Direction.y * m_Speed;
    m_Position.z += m_Direction.z * m_Speed;

    Vector3 toCam = m_Camera.GetPosition() - Vector3(m_Position.x, m_Position.y, m_Position.z);
    float yaw = std::atan2(toCam.x, toCam.z);
    float pitch = std::atan2(toCam.y, std::sqrt(toCam.x * toCam.x + toCam.z * toCam.z));
    m_Rotation.x = yaw;   // yaw
    m_Rotation.y = pitch; // pitch
    m_Rotation.z = 0.0f;

    if (toCam.Length() > 10000.0f) {
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