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
    SetTexture(textureMgr->GetTexture("EnemyNormal.png"));

    m_isDeath = false;
}

void Enemy::Update()
{
    if (IsDeath()) { return; }

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
    SetTexture(textureMgr->GetTexture("EnemyDeath.png"));
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