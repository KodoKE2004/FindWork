#include "Hammer.h"
#include "Application.h"
#include "Collider.h"
#include "Scene.h"
#include <algorithm>

Hammer::Hammer(Camera* cam) : Square(cam)
{
}

void Hammer::Initialize()
{
    Square::Initialize();
    m_MoveValue = m_AttackPosY - m_DefaultPosY;
    m_HitResult.SetHitResult(false);
}

void Hammer::Update()
{
    // 攻撃中かどうか
    if (m_isAttack) 
    {
        // 攻撃中のアニメーション処理
        float p ,w ;
        if (!m_isAttacked) 
        {
            // 攻撃アニメーション処理
            p = (m_TimeAttack.timer <= 0.0f) ?
            1.0f :                                                           // true
            std::clamp(m_TimeAttack.timer / m_TimeAttack.limit, 0.0f, 1.0f); // false
            w = Math::Easing::EaseOutBounce(p);
            m_Position.y = m_DefaultPosY + m_MoveValue * w;
        }
        else
        {
            // キャンセルアニメーション処理
            p = (m_TimeCool.timer <= 0.0f) ? 
            1.0f : std::clamp(m_TimeCool.timer / m_TimeCool.limit, 0.0f, 1.0f);
            w = Math::Easing::EaseInQuint(p);
            m_Position.y = m_AttackPosY  + ( - m_MoveValue ) * w;
        }

        if (p >= 1.0f)
        {
            m_TimeAttack.timer = 0.0f;
            
            if (!m_isAttacked) {
                m_isAttacked = true;
                m_Position.y = m_AttackPosY;
                m_TimeCool.timer = 0.0f;
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
    // 攻撃してない場合
    else 
    {
        if (Input::GetKeyTrigger(VK_RETURN)) { 
            Attack(Application::GetDeltaTime());
            m_isAttack = true;
        }
        // m_isAttackがtureならキー入力を受け付けない
        if (Input::GetKeyPress(VK_A)) {
            m_Position.x -= MoveValue * m_SpeedMass;
        }
        if (Input::GetKeyPress(VK_D)) {
            m_Position.x += MoveValue * m_SpeedMass;
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

    m_TimeAttack.limit = 0.9f * (1.0f - mass);
    m_TimeCool.limit   = 0.6f * (1.0f - mass);

    m_TimeAttack.timer = 0.0f;
    m_isAttack = true;
}