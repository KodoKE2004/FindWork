#include "Debug.hpp"
#include "Game.h"
#include "GameSceneSlice.h"
#include "Math.h"

void GameSceneSlice::Initialize()
{
#ifdef _DEBUG
    DebugUI::TEXT_CurrentScene = "GameSceneSlice";
#endif
    GameSceneExe::SetBaseBeatCount(BASE_BEATS);
    GameSceneExe::Initialize();

    // シーンに繋ぐ情報は基底初期化後の一番最初に設定
    m_RelationData.previousScene = SCENE_NO::GAME_SLICE;
    m_RelationData.oldScene = SCENE_NO::GAME_WAIT;
    m_RelationData.isClear = false;

    auto& instance = Game::GetInstance();
    TextureManager* textureMgr = instance;

    //===============================
    //      シーン内オブジェクト生成
    //===============================
    m_Skydome = instance.AddObject<Skydome>();
    m_Skydome->SetName("m_Skydome");
    m_Skydome->SetSkyDomeMode(true);
    m_Skydome->SetTexture(textureMgr->GetTexture("SkydomeSpace.png"));
    m_Skydome->SetRadius(500.0f);
    m_MySceneObjects.emplace_back(m_Skydome);

    m_Bomber = instance.AddObject<Bomber>();
    m_Bomber->SetName("m_TimeGauge");
    m_MySceneObjects.emplace_back(m_Bomber);

    m_Player = instance.AddObject<Player>();
    m_Player->SetName("m_Player");
    m_Player->SetPos(-200.0f, 0.0f, 1.0f);
    m_Player->SetTexture(textureMgr->GetTexture("Sord.png"));
    m_MySceneObjects.emplace_back(m_Player);

    
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

    std::vector<std::shared_ptr<Enemy>> enemys = instance.GetObjects<Enemy>();
    if (IsAllDeathEnemy(enemys)) {
        // SceneExeで早めにクリアをした場合も想定
        StageClear();
        StageChangeFast();
    }
    else
    {
        // 攻撃判定
        for(auto it : enemys)
        { 
            if (it->IsDeath()) {
                continue;
            }
            m_Player->m_HitResult.SetHitResult(
                isHitSquareSquare(*m_Player, *it)
            );
            
            if (m_Player->m_HitResult.isTriggered() && !it->IsDeath())
            {
                Debug::Log("[[衝突]] Sord -> Enemy");
                it->Death();
            }
        }
    }

    GameSceneExe::Update(tick);

    if (IsChange()) 
    {
        ApplyBeatDuration(GameToWait, m_RelationData);
        ChangeScenePop(GameToWait);
    }

}

void GameSceneSlice::Finalize()
{
    GameSceneExe::Finalize();
}
