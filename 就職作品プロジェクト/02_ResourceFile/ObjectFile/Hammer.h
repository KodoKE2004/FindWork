#pragma once
#include "Square.h"

class Hammer : public Square
{
private:
    bool m_isAttack   = false;
    bool m_isAttacked = false;

    TimerData m_TimeAttack = { 0.0f, 0.4f };
    TimerData m_TimeCool   = { 0.0f, 0.6f };

    float m_SpeedMass     = 1.0f;      // スピード倍率
    const float MoveValue = 4.0f;

    const float m_DefaultPosY =  200.0f; // ハンマーの初期化位置Y座標
    const float m_AttackPosY = - 180.0f; // ハンマーの攻撃位置Y座標
    float m_MoveValue = 0.0f;            // ハンマーの移動量

public:
    Hammer(Camera& cam);
    ~Hammer() override = default ;
    void Initialize() override;
    void Update()     override;
    void Draw()       override;
    void Finalize()   override;

    void Attack(float tick);
    void SetSpeedMass(float speedMass){ m_SpeedMass = speedMass; }

    float* GetTimeAttack() { return &m_TimeAttack.timer; }
    float* GetTimeCool()   { return &m_TimeCool.timer; }

    bool IsAttack() const { return m_isAttack; }
};

