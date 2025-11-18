#pragma once
#include "Square.h"

class Hammer : public Square
{
private:
    bool m_isAttack   = false;
    bool m_isAttacked = false;

    float m_AttackDuration = 1.0f;      // 攻撃中のアニメーション時間    (倍率x 1.0fの場合)
    float m_CoolDuration   = 0.6f;      // 攻撃後に戻るアニメーション時間 (倍率x 1.0fの場合)
    float m_Elapsed        = 0.0f;      // 経過時間
    float m_SpeedMass      = 1.0f;      // スピード倍率
    const float MoveValue = 2.0f;

    const float m_DefaultPosY =  200.0f; // ハンマーの初期化位置Y座標
    const float m_AttackPosY = - 200.0f; // ハンマーの攻撃位置Y座標
    float m_MoveValue = 0.0f;            // ハンマーの移動量

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

