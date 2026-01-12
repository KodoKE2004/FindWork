#include "GameSceneCrush.h"
#include "Game.h"
#include "Calculator.h"
#include "Enemy.h"

using namespace Math::Collider2D;

void GameSceneCrush::Initialize()
{
#ifdef _DEBUG
    DebugUI::TEXT_CurrentScene = "GameSceneCrush";
#endif 

    // 基底クラスの初期化
    GameSceneExe::SetBaseBeatCount(BASE_BEATS);
    GameSceneExe::Initialize();
    // シーンに繋ぐ情報は基底初期化後の一番最初に設定
    m_RelationData.previousScene = SCENE_NO::GAME_CRUSH;
    m_RelationData.oldScene      = SCENE_NO::GAME_WAIT;
    m_RelationData.isClear = false;

    auto& instance  = Game::GetInstance();
    TextureManager* textureMgr = instance;

    m_Bomber = instance.AddObject<Bomber>();
    m_Bomber->SetName("m_Bomber");
    m_MySceneObjects.emplace_back(m_Bomber);
    
    int difficult = m_RelationData.stageCount / 4;
    if (difficult >= 4){ difficult = 3; }
    for (int i = 0; i <= difficult; ++i)
    {
        auto enemy = instance.AddObject<Enemy>();
        enemy->SetName("m_Enemy");
        enemy->SetPos  ( 0.0f, - 180.0f, 1.0f);
        m_MySceneObjects.emplace_back(enemy);
    }

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
        ApplyBeatDuration(GameToWait, m_RelationData);
        ChangeScenePop(GameToWait);
    }
}

void GameSceneCrush::Finalize()
{
    GameSceneExe::Finalize();
}
