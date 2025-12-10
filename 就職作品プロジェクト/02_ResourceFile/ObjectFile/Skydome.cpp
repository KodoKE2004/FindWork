#include "Skydome.h"
#include "../Game.h"

Skydome::Skydome(Camera& cam) : Sphere(cam)
{
    m_Texture = nullptr;
}

void Skydome::Initialize() {

    Sphere::Initialize();

    SetShader("VS_Unlit","PS_Unlit");
    
}

void Skydome::Spin(float spinX, float spinY, float spinZ)
{
    // 意図してるデフォルトの値が低いので倍率調整
    // あくまで引数では直感的に
    spinX *= 0.0001f;
    spinY *= 0.0001f;
    spinZ *= 0.0001f;

    // 回転方向の設定
    auto camPos = m_Camera.GetPosition();
    m_Position = NVector3(camPos.x, camPos.y, camPos.z);
    m_Rotation = NVector3(GetRotate().x + spinX, GetRotate().y + spinY, GetRotate().z + spinZ);

    float dt = Application::GetDeltaTime();
    // 回転をデルタタイム依存で滑らかに
    m_Rotation.x += m_SkyRotationSpeed.x * dt;
    m_Rotation.y += m_SkyRotationSpeed.y * dt;
    m_Rotation.z += m_SkyRotationSpeed.z * dt;

    // 2PI でラップ(fmod を使用)
    const float twoPI = DirectX::XM_2PI;
    m_Rotation.x = std::fmod(m_Rotation.x, twoPI);
    m_Rotation.y = std::fmod(m_Rotation.y, twoPI);
    m_Rotation.z = std::fmod(m_Rotation.z, twoPI);
}

void Skydome::Spin(const NVector3& spinVec3)
{
    Spin(spinVec3.x, spinVec3.y, spinVec3.z);
}
