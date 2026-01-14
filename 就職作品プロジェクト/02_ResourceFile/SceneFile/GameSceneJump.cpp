#include "GameSceneJump.h"
#include "Game.h"
#include "Calculator.h"
#include "DebugUI.h"
#include <random>
#include "GameSceneText.h"

float GameSceneJump::GenerateActivationDelay()
{

    static std::mt19937 engine{ std::random_device{}() };
    std::uniform_real_distribution<float> dist(1.0f, 2.0f);

    return dist(engine);
}

void GameSceneJump::Initialize()
{
    DebugUI::TEXT_CurrentScene = "GameSceneJump";

    // 基底クラスの初期化
    GameSceneExe::SetBaseBeatCount(BASE_BEATS);
    GameSceneExe::Initialize();

    // シーンに繋ぐ情報は基底初期化後の一番最初に設定
    m_RelationData.previousScene = SCENE_NO::GAME_JUMP;
    m_RelationData.oldScene = SCENE_NO::GAME_WAIT;
    m_RelationData.isClear = true;

    auto& instance = Game::GetInstance();
    TextureManager* textureMgr = instance;

    m_Bomber = instance.AddObject<Bomber>();
    m_Bomber->SetName("m_TimeGauge");
    m_MySceneObjects.emplace_back(m_Bomber);

    m_Bird = instance.AddObject<Bird>();
    m_MySceneObjects.emplace_back(m_Bird);

    m_HasSpawnedCartWarning = false;
}

void GameSceneJump::Update(float tick)
{
 
   
    GameSceneExe::Update(tick);
    if (IsChange()) 
    {
        ApplyBeatDuration(GameToWait, m_RelationData);
        ChangeScenePop(GameToWait);
    }


}

void GameSceneJump::Finalize()
{
    GameSceneExe::Finalize();
}

