#pragma once
#include "Square.h"
#include <vector>
class Enemy : public Square
{
private:
    bool m_isDeath    = false;
    bool m_isLifeSpan = false; // 寿命フラグ
    int  m_HP      = 10;          // 敵のHP
    float m_DeathElapsed = 0.0f;  // 死亡してからの経過時間
    float m_DeathDuration = 3.0f; // 死亡アニメーションの継続時間
public:
    Enemy(Camera & cam);
    ~Enemy() = default ;

    void Initialize() override;
    void Update()     override;
    void Draw()       override;
    void Finalize()   override;
    
    void SetHP(int hp) {
        m_HP = hp;
    }
    int GetHP() const {
        return m_HP;
    }
    void Damage(int damage) {
        m_HP -= damage;
        if (m_HP <= 0) {
            Death();
        }
    }
    void Death() ;
    bool IsDeath() {
        return m_isDeath;
    }
    void SetLifeSpan(bool isLifeSpan) {
        m_isLifeSpan = isLifeSpan;
    }
    bool IsLifeSpan() const {
        return m_isLifeSpan;
    }
};

bool IsAllDeathEnemy(vector<pShared<Enemy>> enemys);
