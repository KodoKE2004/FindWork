#include "GameSceneSlice.h"
#include "Game.h"
#include "Calculator.h"
#include "Debug.hpp"

void GameSceneSlice::Initialize()
{
#ifdef _DEBUG
    DebugUI::TEXT_CurrentScene = "GameSceneSlice";
#endif

    GameSceneExe::Initialize();

    // シーンに繋ぐ情報は基底初期化後の一番最初に設定
    m_RelationData.isClear = false;

    // リズムの定義
    RhythmBeatConst beatConfig{};
    auto& rhythmBeat = Game::GetRhythmBeat();
    beatConfig.Setup(Game::GetBgmBpm(), 4, 16);
    rhythmBeat.Initialize(beatConfig, false, BASE_BEATS);

    auto& instance = Game::GetInstance();
    TextureManager* textureMgr = instance;

    //===============================
    //      シーン内オブジェクト生成
    //===============================
    m_Bomber = instance.AddObject<Bomber>();
    m_Bomber->SetName("m_TimeGauge");
    m_MySceneObjects.emplace_back(m_Bomber);

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
    using namespace Calculator::Collider2D; 

    auto& instance = Game::GetInstance();

    std::vector<std::shared_ptr<Enemy>> enemys = instance.GetObjects<Enemy>();
    if (IsAllDeathEnemy(enemys)) {
        // SceneExeで早めにクリアをした場合も想定
        StageClear();
        SetFastChange();
    }
    else
    {
        for(auto it : enemys)
        { 
            if (it->IsDeath()) {
                continue;
            }
        }
    }

    GameSceneExe::Update(tick);

    if (IsChange()) 
    {
        ChangeScenePop(GameToWait);
    }

}

void GameSceneSlice::Finalize()
{
    GameSceneExe::Finalize();
}
