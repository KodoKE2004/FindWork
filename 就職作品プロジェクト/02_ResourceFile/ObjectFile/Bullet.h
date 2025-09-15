#pragma once
#include "../Texture2D.h"

class Bullet : public Texture2D
{
public:
    Bullet(Camera* cam);

    void Initialize() override;
    void Update() override;
    void Draw() override;
    void Finalize() override;

    void Shoot(const DirectX::SimpleMath::Vector3& pos,
        const DirectX::SimpleMath::Vector3& dir);

    bool IsAlive() const { return m_IsAlive; }

private:
    DirectX::SimpleMath::Vector3 m_Direction; // movement direction
    float m_Speed;                            // movement speed
    bool m_IsAlive;                           // active flag

};