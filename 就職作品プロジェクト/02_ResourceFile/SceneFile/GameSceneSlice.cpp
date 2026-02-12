#include "GameSceneSlice.h"
#include "Game.h"
#include "Calculator.h"
#include "Debug.hpp"

GameSceneSlice::GameSceneSlice(std::shared_ptr<Camera> cam) : GameSceneExe(cam)
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
    beatConfig.Setup(Game::GetBgmBpm(), 8, 4, 16, 4);
    rhythmBeat.Initialize(beatConfig, false, BASE_BEATS);

    // ゲーム内の総拍数を参照するためリズム定義より後
    GameSceneExe::Initialize();

    auto& instance = Game::GetInstance();
    auto  camera   = instance.GetCamera();
    TextureManager* textureMgr = instance;

    //===============================
    //      シーン内オブジェクト生成
    //===============================
    m_Bomber = AddObject<Bomber>(camera);
    m_Bomber->SetName("m_TimeGauge");
    
    int difficult = m_RelationData.stageCount / 4;
    for (int i = 0; i <= difficult; ++i)
    {
        auto enemy = AddObject<Enemy>(camera);
        enemy->SetName("m_Enemy");
        enemy->SetTexture(textureMgr->GetTexture("EnemyNormal.png"));
        enemy->SetPos(200.0f, -100.0f, 0.0f);
        enemy->SetScale(100.0f, 100.0f, 1.0f);
    }
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
