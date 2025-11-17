#include "GameSceneHit.h"
#include "Game.h"
#include "Collider.h"
using namespace Math::Collider2D;

void GameSceneHit::Initialize()
{
    DebugUI::TEXT_CurrentScene = "GameSceneHit";

    auto& instance  = Game::GetInstance();
    TextureManager* textureMgr = instance;
    
    m_RelationData.previousScene = SCENE_NO::GAME_HIT;
    m_RelationData.nextScene     = SCENE_NO::GAME_WAIT;


    m_Skydome = instance.AddObject<Skydome>();
    m_Skydome->SetName("m_Skydome");
    m_Skydome->SetSkyDomeMode(true);
    m_Skydome->SetTexture(textureMgr->GetTexture("SkydomeSpace.png"));
    m_Skydome->SetRadius(500.0f);
    m_MySceneObjects.emplace_back(m_Skydome);

    m_Hammer = instance.AddObject<Hammer>();
    m_Hammer->SetName("m_Hammer");
    m_Hammer->SetTexture(textureMgr->GetTexture("Plane.png"));
    m_Hammer->SetSpeedMass(m_GameSpeedMass);
    m_MySceneObjects.emplace_back(m_Hammer);
    
    int difficult = m_RelationData.stageCount % 4;
    for (int i = 0; i < difficult; ++i)
    {
        auto enemy = instance.AddObject<Enemy>();
        enemy->SetName("m_Enemy");
        enemy->SetTexture(textureMgr->GetTexture("Battle_EnemyNormal.png"));
        enemy->SetPos  (   0.0f, - 200.0f, 1.0f);
        m_MySceneObjects.emplace_back(enemy);
    }
}

void GameSceneHit::Update(float tick)
{
    auto& instance = Game::GetInstance();
    auto enemys = instance.GetObjects<Enemy>();
    if (enemys.size() == 0) {
        // SceneExe‚Å‘‚ß‚ÉƒNƒŠƒA‚ð‚µ‚½ê‡‚à‘z’è
        m_isFastChange = true;
        m_RelationData.isClear = true;
    }
    else
    {
        for (auto it : enemys)
        {
            m_Hammer->m_HitResult.SetHitResult(
                isHitSquareSquare(*m_Hammer, *it)
            );

            if (m_Hammer->m_HitResult.isTriggered() && !it->IsDeath())
            {
                MyDebugLog(Debug::Log("“–‚½‚Á‚½");)
                it->Death();
                m_RelationData.isClear = true;
                m_RelationData.stageCount += 2;
            }
        }
    }


    GameSceneExe::Update(tick);
}

void GameSceneHit::Finalize()
{
    GameSceneExe::Finalize();
}
