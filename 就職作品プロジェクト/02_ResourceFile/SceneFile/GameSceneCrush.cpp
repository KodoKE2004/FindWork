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
    TextureManager* textureMgr = instance;

    // シーンに繋ぐ情報は基底初期化後の一番最初に設定
    m_RelationData.isClear = false;

    // リズムの定義
    RhythmBeatConst beatConfig{};
    auto& rhythmBeat = Game::GetRhythmBeat();
    beatConfig.Setup(Game::GetBgmBpm());
    rhythmBeat.Initialize(beatConfig, false, BASE_BEATS);
    
    // ゲーム内の総拍数を参照するためリズム定義より後
    GameSceneExe::Initialize();

    m_Skydome = AddObject<Skydome>(instance.GetCamera());
    m_Skydome->SetName("m_Skydome");
    m_Skydome->SetSkyDomeMode(true);
    m_Skydome->SetTexture(textureMgr->GetTexture("SkydomeSpace.png"));
    m_Skydome->SetRadius(5.0f);
    
    m_Player = AddObject<Player>(instance.GetCamera());
    //m_Player->SetPos();
    //m_Player->SetScale();
    int difficult = m_RelationData.stageCount / 4;
    if (difficult >= 4){ difficult = 3; }
    for (int i = 0; i <= difficult; ++i)
    {
        auto enemy = AddObject<Enemy>(instance.GetCamera());
        enemy->SetName("m_Enemy");
        enemy->SetPos  ( 0.0f, - 180.0f, 1.0f);
    }

    m_Bomber = AddObject<Bomber>(instance.GetCamera());
    m_Bomber->SetName("m_TimeGauge");
    m_MySceneObjects.emplace_back(m_Bomber->GetRope());
    m_MySceneObjects.emplace_back(m_Bomber->GetNumber());
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
    
    // Skydomeの回転
    m_Skydome->Spin(0.0f, -4.0f, 0.0f);

    if (!m_RelationData.isClear) 
    {

    }



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
