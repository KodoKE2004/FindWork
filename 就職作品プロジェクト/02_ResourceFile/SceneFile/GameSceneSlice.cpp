#include "GameSceneSlice.h"
#include "Game.h"
#include "Calculator.h"
#include "Debug.hpp"

GameSceneSlice::GameSceneSlice(Camera& cam) : GameSceneExe(cam)
{
}

void GameSceneSlice::Initialize()
{
#ifdef _DEBUG
    DebugUI::TEXT_CurrentScene = "GameSceneSlice";
#endif
    // シーンに繋ぐ情報は基底初期化後の一番最初に設定
    m_RelationData.isClear = false;

    // リズムの定義
    RhythmBeatConst beatConfig{};
    auto& rhythmBeat = Game::GetRhythmBeat();
    beatConfig.Setup(Game::GetBgmBpm());
    rhythmBeat.Initialize(beatConfig, false, BASE_BEATS);

    // ゲーム内の総拍数を参照するためリズム定義より後
    GameSceneExe::Initialize();

    auto& instance = Game::GetInstance();
    TextureManager* textureMgr = instance;

    //===============================
    //      シーン内オブジェクト生成
    //===============================
    
    int difficult = m_RelationData.stageCount / 4;
    for (int i = 0; i <= difficult; ++i)
    {
        auto enemy = AddObject<Enemy>(instance.GetCamera());
        enemy->SetName("m_Enemy");
        enemy->SetTexture(textureMgr->GetTexture("EnemyNormal.png"));
        enemy->SetPos(200.0f, -100.0f, 0.0f);
        enemy->SetScale(100.0f, 100.0f, 1.0f);
    }

    m_Bomber = AddObject<Bomber>(instance.GetCamera());
    m_Bomber->SetName("m_TimeGauge");
    m_MySceneObjects.emplace_back(m_Bomber->GetRope());
    m_MySceneObjects.emplace_back(m_Bomber->GetNumber());
}

void GameSceneSlice::Update(float tick)
{
    using namespace Calculator::Collider2D; 

    std::vector<std::shared_ptr<Enemy>> enemys = GetObjects<Enemy>();
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
        ChangeScene();
    }

}

void GameSceneSlice::Draw()
{
    Scene::Draw();
}

void GameSceneSlice::Finalize()
{
    GameSceneExe::Finalize();
}
