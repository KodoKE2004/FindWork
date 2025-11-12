#pragma once
#include "Square.h"

class Hammer : public Square
{
private:
    bool  m_isAttack = false;

    float m_AttackTime     = 0.5f;      // 攻撃中のアニメーション時間    (倍率x 1.0fの場合)
    float m_AttackCoolTime = 0.3f;      // 攻撃後に戻るアニメーション時間 (倍率x 1.0fの場合)
    float m_Duration       = 0.0f;      // 経過時間
    float m_SpeedMass      = 1.0f;      // スピード倍率

    const float m_DefaultPosY =  100.0f; // ハンマーの初期化位置Y座標
    const float m_AttackPosY = - 200.0f; // ハンマーの攻撃位置Y座標
    
public:
    Hammer(Camera* cam);
    ~Hammer() override = default ;
    void Initialize() override;
    void Update()     override;
    void Draw()       override;
    void Finalize()   override;

    void Attack(float tick);
    void SetSpeedMass(float speedMass){ m_SpeedMass = speedMass; }
};

