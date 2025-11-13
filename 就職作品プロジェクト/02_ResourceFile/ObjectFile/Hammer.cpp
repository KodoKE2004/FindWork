#include "Hammer.h"
#include "Application.h"
#include "Collider.h"
#include <algorithm>

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
    if (m_isAttack) 
    {
        // 攻撃中のアニメーション処理
        m_Duration += Application::GetDeltaTime();
        float p ,w ;
        if (!m_isAttacked) 
        {
            // 攻撃アニメーション処理
            p = (m_AttackTime <= 0.0f) ? 1.0f : std::clamp( m_Duration / m_AttackTime, 0.0f, 1.0f);
            w = Math::Easing::EaseOutBounce(p);
            m_Position.y = m_DefaultPosY + m_MoveValue * w;
        }
        else
        {
            // キャンセルアニメーション処理
            p = (m_AttackTime <= 0.0f) ? 1.0f : std::clamp(m_Duration / m_AttackCoolTime, 0.0f, 1.0f);
            w = Math::Easing::EaseOutQuart(p);
            m_Position.y = m_AttackPosY  + ( - m_MoveValue ) * w;
        }

        if (p >= 1.0f)
        {
            m_Duration = 0.0f;
            
            if (!m_isAttacked) {
                m_isAttacked = true;
                m_Position.y = m_AttackPosY;
            }
            else
            {
                m_isAttacked = false;
                m_isAttack   = false;
                m_Position.y = m_DefaultPosY;
            }
        }
                
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

    m_AttackTime     = 0.9f * (1.0f - mass);
    m_AttackCoolTime = 0.6f * (1.0f - mass);

    m_Duration = 0.0f;
    m_isAttack = true;
}