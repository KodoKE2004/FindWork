#include "GameSceneCrush.h"
#include "Game.h"
#include "Collider.h"
#include "Enemy.h"

using namespace Math::Collider2D;

void GameSceneCrush::Initialize()
{
#ifdef _DEBUG
    DebugUI::TEXT_CurrentScene = "GameSceneCrush";
#endif 

    // 基底クラスの初期化
    GameSceneExe::Initialize();
    // シーンに繋ぐ情報は基底初期化後の一番最初に設定
    m_RelationData.previousScene = SCENE_NO::GAME_CRUSH;
    m_RelationData.oldScene      = SCENE_NO::GAME_WAIT;
    m_RelationData.isClear = false;

    auto& instance  = Game::GetInstance();
    TextureManager* textureMgr = instance;

    m_Skydome = instance.AddObject<Skydome>();
    m_Skydome->SetName("m_Skydome");
    m_Skydome->SetSkyDomeMode(true);
    m_Skydome->SetTexture(textureMgr->GetTexture("SkydomeSpace.png"));
    m_Skydome->SetRadius(500.0f);
    m_MySceneObjects.emplace_back(m_Skydome);

    m_Bomber = instance.AddObject<Bomber>();
    m_Bomber->SetName("m_Bomber");
    m_MySceneObjects.emplace_back(m_Bomber);
    
    m_Hammer = instance.AddObject<Hammer>();
    m_Hammer->SetName("m_Hammer");
    m_Hammer->SetPos(0.0f, 200.0f, 1.0f);
    m_Hammer->SetTexture(textureMgr->GetTexture("Weight.png"));
    m_MySceneObjects.emplace_back(m_Hammer);
    
    int difficult = m_RelationData.stageCount / 4;
    if (difficult >= 4){ difficult = 3; }
    for (int i = 0; i <= difficult; ++i)
    {
        auto enemy = instance.AddObject<Enemy>();
        enemy->SetName("m_Enemy");
        enemy->SetPos  ( 0.0f, - 180.0f, 1.0f);
        m_MySceneObjects.emplace_back(enemy);
    }

    SetTimer(m_Hammer->GetTimeAttack());
    SetTimer(m_Hammer->GetTimeCool());

}

void GameSceneCrush::Update(float tick)
{
    auto& instance = Game::GetInstance();
    auto enemys = instance.GetObjects<Enemy>();
    if (IsAllDeathEnemy(enemys)) {
        // SceneExeで早めにクリアをした場合も想定
        m_isFastChange = true;
        m_RelationData.isClear = true;
    }

    GameSceneExe::Update(tick);

    if (IsChange()) 
    {
        const float beatMultiplier = (IsFastChange() && m_RelationData.isClear) ? 0.7f : 1.0f;
        ApplyBeatDuration(GameToWait, m_RelationData, beatMultiplier);
        ChangeScenePop(GameToWait);
    }
}

void GameSceneCrush::Finalize()
{
    GameSceneExe::Finalize();
}
