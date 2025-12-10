#include "Enemy.h"
#include "Game.h"
#include "Collider.h"
#include "Player.h"
#include "Sord.h"
#include "Hammer.h"
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
    if (IsDeath()) 
        return;

    auto& instance = Game::GetInstance();
    
    //-------------------------------
    //          ìñÇΩÇËîªíËèàóù
    //-------------------------------

    switch (Scene::m_RelationData.previousScene)
    {
    case SCENE_NO::GAME_CRUSH:
    {
        auto hammer = instance.GetObjects<Hammer>();
        if (!hammer.empty())
        {
            bool isHit = Math::Collider2D::isHitSquareSquare(*hammer[0], *this);
            if (isHit && hammer[0]->IsAttack())
            {
                Debug::Log("[è’ìÀ] Enemy - Hammer");
                Death();                 
            }
        }
    }
    break;
    case SCENE_NO::GAME_SLICE:
    {
        auto sord = instance.GetObjects<Sord>();
        if (!sord.empty())
        {
            bool isHit = Math::Collider2D::isHitSquareSquare(*sord[0], *this);
            if (isHit)
            {
                Debug::Log("[è’ìÀ] Enemy - Sord");
                Death();                 
            }
        }
    }
    break;
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
    SetTexture(textureMgr->GetTexture("EnemyDeath.png"));
    m_isDeath = true;
}

bool IsAllDeathEnemy(std::vector<Enemy*> enemys)
{
    for (Enemy* it : enemys)
    {
        if (!it->IsDeath()) {
            return false;
        }
    }
    return true;
}