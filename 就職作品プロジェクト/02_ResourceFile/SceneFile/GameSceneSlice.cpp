#include "GameSceneSlice.h"
#include "Game.h"
#include "Collider.h"
#include "Debug.hpp"

void GameSceneSlice::Initialize()
{
#ifdef _DEBUG
    DebugUI::TEXT_CurrentScene = "GameSceneSlice";
#endif
    
    GameSceneExe::Initialize();

    // シーンに繋ぐ情報は基底初期化後の一番最初に設定
    m_RelationData.previousScene = SCENE_NO::GAME_SLICE;
    m_RelationData.oldScene = SCENE_NO::GAME_WAIT;
    m_RelationData.isClear = false;

    auto& instance = Game::GetInstance();
    TextureManager* textureMgr = instance;

    m_Skydome = instance.AddObject<Skydome>();
    m_Skydome->SetName("m_Skydome");
    m_Skydome->SetSkyDomeMode(true);
    m_Skydome->SetTexture(textureMgr->GetTexture("SkydomeSpace.png"));
    m_Skydome->SetRadius(500.0f);
    m_MySceneObjects.emplace_back(m_Skydome);

    m_TimeGaugeBack = instance.AddObject<Bomber>();
    m_MySceneObjects.emplace_back(m_TimeGaugeBack);

    m_Sord = instance.AddObject<Sord>();
    m_Sord->SetName("m_Sord");
    m_Sord->SetPos(-200.0f, 0.0f, 1.0f);
    m_Sord->SetTexture(textureMgr->GetTexture("Sord.png"));
    m_MySceneObjects.emplace_back(m_Sord);

    
    int difficult = m_RelationData.stageCount / 4;
    for (int i = 0; i <= difficult; ++i)
    {
        auto enemy = instance.AddObject<Enemy>();
        enemy->SetName("m_Enemy");
        enemy->SetTexture(textureMgr->GetTexture("EnemyNormal.png"));
        enemy->SetPos(200.0f, -100.0f, 0.0f);
        enemy->SetScale(100.0f, 100.0f, 1.0f);
        m_MySceneObjects.emplace_back(enemy);
    }

   
    
}

void GameSceneSlice::Update(float tick)
{
    using namespace Math::Collider2D; 

    auto& instance = Game::GetInstance();

    std::vector<Enemy*> enemys = instance.GetObjects<Enemy>();
    if (IsAllDeathEnemy(enemys)) {
        // SceneExeで早めにクリアをした場合も想定
        m_isFastChange = true;
         m_RelationData.isClear = true;
    }
    else
    {
        // 攻撃判定
        for(auto it : enemys)
        { 
            m_Sord->m_HitResult.SetHitResult(
                isHitSquareSquare(*m_Sord, *it)
            );
            
            if (m_Sord->m_HitResult.isTriggered() && !it->IsDeath()) 
            {
                MyDebugLog(Debug::Log("当たった");)
                it->Death();
            }
        }
    }

    GameSceneExe::Update(tick);

    if (IsChange()) {
        ChangeScenePop(GameToWait);
    }

}

void GameSceneSlice::Finalize()
{
    GameSceneExe::Finalize();
}
