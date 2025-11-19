#include "Enemy.h"
#include "Game.h"

Enemy::Enemy(Camera* cam) : Square(cam)
{
}

void Enemy::Initialize()
{   
    Square::Initialize();
    m_isDeath = false;
}

void Enemy::Update()
{
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
    TextureManager* textureMgr = Game::GetInstance();
    SetTexture(textureMgr->GetTexture("Battle_EnemyDeath.png"));
    m_isDeath = true;
}

bool IsAllDeathEnemy(std::vector<Enemy*> enemys)
{
    for (Enemy* it : enemys)
    {
        if (it->IsDeath() == false) {
            return false;
        }
    }
    return true;
}