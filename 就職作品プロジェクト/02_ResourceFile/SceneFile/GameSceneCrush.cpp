#include "GameSceneCrush.h"
#include "Game.h"
#include "Calculator.h"
#include "Enemy.h"

using namespace Calculator::Collider2D;

GameSceneCrush::GameSceneCrush(Camera& cam) : GameSceneExe(cam)
{
}

void GameSceneCrush::Initialize()
{
#ifdef _DEBUG
    DebugUI::TEXT_CurrentScene = "GameSceneCrush";
#endif 
    auto& instance = Game::GetInstance();
    auto  camera   = instance.GetCamera();

    // シーンに繋ぐ情報は基底初期化後の一番最初に設定
    m_RelationData.isClear = false;

    // リズムの定義
    RhythmBeatConst beatConfig{};
    auto& rhythmBeat = Game::GetRhythmBeat();
    beatConfig.Setup(Game::GetBgmBpm(), 8, 4, 16, 4);
    rhythmBeat.Initialize(beatConfig, false, BASE_BEATS);
    
    // ゲーム内の総拍数を参照するためリズム定義より後
    GameSceneExe::Initialize();

    m_Bomber = AddObject<Bomber>(camera);
    m_Bomber->SetName("m_Bomber");
    
    int difficult = m_RelationData.stageCount / 4;
    if (difficult >= 4){ difficult = 3; }
    for (int i = 0; i <= difficult; ++i)
    {
        auto enemy = AddObject<Enemy>(camera);
        enemy->SetName("m_Enemy");
        enemy->SetPos  ( 0.0f, - 180.0f, 1.0f);
    }
}

void GameSceneCrush::Update(float tick)
{
    auto enemys = GetObjects<Enemy>();
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

void GameSceneCrush::Draw()
{
    Scene::Draw();
}

void GameSceneCrush::Finalize()
{
    GameSceneExe::Finalize();
}
