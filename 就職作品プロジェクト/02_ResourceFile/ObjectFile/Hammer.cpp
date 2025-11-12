#include "Hammer.h"
#include "Application.h"
#include "Collider.h"

Hammer::Hammer(Camera* cam) : Square(cam)
{
}

void Hammer::Initialize()
{
    Square::Initialize();
    m_MoveValue = m_AttackPosY - m_DefaultPosY;
}

void Hammer::Update()
{
    // 攻撃中かどうか
    if (m_isAttack) {
        // 攻撃中のアニメーション処理
        m_Duration += Application::GetDeltaTime();
        float progress = m_Duration / m_AttackTime;
        
        return;
    }   
    // 攻撃してないとの場合
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
    float mass = 1.0f - m_SpeedMass;
    if (mass < 0.3f) {
        mass = 0.3f;
    }

    m_AttackTime     *= (1.0f - mass);
    m_AttackCoolTime *= (1.0f - mass);



    m_Duration = 0.0f;
    m_isAttack = true;
}