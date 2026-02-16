#include "Enemy.h"
#include "Game.h"
#include "Calculator.h"
#include "Player.h"
#include "Scene.h"

Enemy::Enemy(Camera& cam) : Square(cam)
{
}

void Enemy::Initialize()
{   
    TextureManager* textureMgr = Game::GetInstance();
    Square::Initialize();
    SetTexture(textureMgr->GetTexture("GameScene/GameEnemy.png"));
    m_DeathDuration = 2.0f; // 死亡アニメーションの継続時間を設定
    m_isDeath = false;
}

void Enemy::Update()
{
    if (IsDeath()) {
        m_Rotation.z += 0.1f; // 死亡後は回転させる
        m_DeathElapsed += static_cast<float>(Application::GetDeltaTime());
        if (m_DeathElapsed >= m_DeathDuration) {
            SetLifeSpan(true);  // 寿命フラグを立てる
        }
        return; 
    }

}

void Enemy::Draw()
{
    Square::Draw();
}

void Enemy::Finalize()
{
    Square::Finalize();
}

void Enemy::Death()
{
    if (m_isDeath) {
        return;
    }

    TextureManager* textureMgr = Game::GetInstance();
    SetTexture(textureMgr->GetTexture("Bomber/Explosion.png"));
    m_isDeath = true;
}

bool IsAllDeathEnemy(vector<pShared<Enemy>> enemys)
{
    for (pShared<Enemy> it : enemys)
    {
        if (!it->IsDeath()) {
            return false;
        }
    }
    return true;
}