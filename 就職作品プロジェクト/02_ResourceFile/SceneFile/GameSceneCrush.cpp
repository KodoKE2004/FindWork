#include "GameSceneCrush.h"
#include "Game.h"
#include "Calculator.h"
#include "Enemy.h"

using namespace Calculator::Collider2D;

void GameSceneCrush::Initialize()
{
#ifdef _DEBUG
    DebugUI::TEXT_CurrentScene = "GameSceneCrush";
#endif 


    // シーンに繋ぐ情報は基底初期化後の一番最初に設定
    m_RelationData.isClear = false;

    // リズムの定義
    RhythmBeatConst beatConfig{};
    auto& rhythmBeat = Game::GetRhythmBeat();
    beatConfig.Setup(Game::GetBgmBpm(), 4, 16);
    rhythmBeat.Initialize(beatConfig, false, BASE_BEATS);
    
    // ゲーム内の総拍数を参照するためリズム定義より後
    GameSceneExe::Initialize();

    auto& instance  = Game::GetInstance();
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
        ChangeScene();
    }

}

void GameSceneCrush::Finalize()
{
    GameSceneExe::Finalize();
}
