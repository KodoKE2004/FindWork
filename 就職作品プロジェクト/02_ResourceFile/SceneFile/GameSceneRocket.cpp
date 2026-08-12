#include "GameSceneRocket.h"
#include "Game.h"
#include <algorithm>
#include <random>

GameSceneRocket::GameSceneRocket(Camera& cam) : GameSceneExe(cam)
{
}

void GameSceneRocket::Initialize()
{   
#ifdef _DEBUG
    DebugUI::TEXT_CurrentScene = "GameSceneRocket";
#endif
    // シーンに繋ぐ情報は基底初期化後の一番最初に設定
    StageFail();

    // リズムの定義
    RhythmBeatConst beatConfig{};
    beatConfig.Setup(Game::GetBgmBpm());
    auto& session = Game::GetGameplaySession();
    session.InitializeRhythm(beatConfig, false, BASE_BEATS - 8);

    auto& instance = Game::GetInstance();

    m_Skydome = AddObject<Skydome>(instance.GetCamera());

    auto cube = AddObject<Cube>(instance.GetCamera());
    cube->SetName("Cube");
    cube->SetColor(Color(0.5f, 0.5f, 0.5f, 1.0f));
    
    m_Bomber = AddObject<Bomber>(instance.GetCamera());
    m_Bomber->SetName("m_TimeGauge");

    m_MySceneObjects.emplace_back(m_Bomber->GetRope());
    m_MySceneObjects.emplace_back(m_Bomber->GetNumber());

    // ゲーム内の総拍数を参照するためリズム定義より後
    GameSceneExe::Initialize();

    PlayParams bgmParams;
    m_AudioList.emplace("bgmRocket", AudioConfig(L"BGM/GameSceneMelody/Rocket.wav", bgmParams, true, true));

    PlayParams clearParams;
    m_AudioList.emplace("clear", AudioConfig(L"SE/GameReaction/True2.wav", clearParams, false, false));

    RegisterReactionSE("clear");
    RegisterAudio();
}

void GameSceneRocket::Update(float tick)
{
    // デバッグ中
    GameSceneExe::Update(tick);
}

void GameSceneRocket::Draw()
{
    for (auto object : m_MySceneObjects)
    {
        object->Draw();
    }
}

void GameSceneRocket::Finalize()
{
    // このシーンのオブジェクトを削除する
    for (auto o : m_MySceneObjects) {
        DeleteObject(o);
    }
    m_MySceneObjects.clear();
    GameSceneExe::Finalize();
}
