#include "Enemy.h"
#include "Game.h"

Enemy::Enemy(Camera* cam) : Square(cam)
{
}

void Enemy::Initialize()
{   
    Square::Initialize();
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
