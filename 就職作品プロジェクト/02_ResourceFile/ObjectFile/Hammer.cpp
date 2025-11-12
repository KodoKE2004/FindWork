#include "Hammer.h"
#include "Application.h"

Hammer::Hammer(Camera* cam) : Square(cam)
{
}

void Hammer::Initialize()
{
    Square::Initialize();

}

void Hammer::Update()
{
    if (m_isAttack) {
        // 攻撃中のアニメーション処理
        m_Duration += Application::GetDeltaTime();

        return;
    }
    else 
    {
        if (Input::GetKeyTrigger(VK_RETURN)) { 
            Attack(Application::GetDeltaTime());
            m_isAttack = true;
        }
        // m_isAttackがtureならキー入力を受け付けない
        if (Input::GetKeyPress(VK_A)) {
            m_Position.x -= 1.0f * m_SpeedMass;
        }
        if (Input::GetKeyPress(VK_D)) {
            m_Position.x += 1.0f * m_SpeedMass;
        }
    }
}

void Hammer::Draw()
{
    Square::Draw();
}

void Hammer::Finalize()
{
    Square::Finalize();
}


void Hammer::Attack(float tick)
{   
    m_AttackTime     *= (1.0f - m_SpeedMass);
    m_AttackCoolTime *= (1.0f - m_SpeedMass);

    m_Duration = 0.0f;
    m_isAttack = true;
}