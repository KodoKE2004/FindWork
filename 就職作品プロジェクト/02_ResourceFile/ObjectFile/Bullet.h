#pragma once
#include "Square.h"

class Bullet : public Square
{
private:
    DirectX::SimpleMath::Vector3 m_Direction; // •ûŒü (³‹K‰»)
    float m_Speed;                            // ’e‚Ì‘¬“x
    bool m_IsAlive;                           // ’e‚Ì¶‘¶ŠÔ
public:
    Bullet(Camera & cam);

    void Initialize() override;
    void Update() override;
    void Draw() override;
    void Finalize() override;

    void Shoot(const DirectX::SimpleMath::Vector3& pos,
        const DirectX::SimpleMath::Vector3& dir);

    bool IsAlive() const { return m_IsAlive; }

    void Alive() {
        m_IsAlive = true;
    }

    void DeAlive() {
        m_IsAlive = false;
    }


};