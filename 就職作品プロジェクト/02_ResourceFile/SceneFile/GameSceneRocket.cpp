#include "GameSceneRocket.h"
#include "Game.h"

GameSceneRocket::GameSceneRocket(Camera& cam) : GameSceneExe(cam)
{
}

void GameSceneRocket::Initialize()
{   
    DebugUI::TEXT_CurrentScene = "GameSceneDodge";

    // シーンに繋ぐ情報は基底初期化後の一番最初に設定
    StageClear();

    // リズムの定義
    RhythmBeatConst beatConfig{};
    auto& rhythmBeat = Game::GetRhythmBeat();
    beatConfig.Setup(Game::GetBgmBpm());
    rhythmBeat.Initialize(beatConfig, false, BASE_BEATS - 8);

    // ゲーム内の総拍数を参照するためリズム定義より後
    GameSceneExe::Initialize();

    auto& instance = Game::GetInstance();
    TextureManager* textureMar = instance;

    m_Background = AddObject<Square>(instance.GetCamera());
    m_Background->SetName("m_Background");
    m_Background->SetScale(1280.0f, 720.0f, 1.0f);
    m_Background->SetTexture(textureMar->GetTexture("BackGround/Rocket.png"));



    m_Bomber = AddObject<Bomber>(instance.GetCamera());
    m_Bomber->SetName("m_TimeGauge");

    m_MySceneObjects.emplace_back(m_Bomber->GetRope());
    m_MySceneObjects.emplace_back(m_Bomber->GetNumber());

    PlayParams bgmParams;
    m_AudioList.emplace("bgmRocket", AudioConfig(L"BGM/GameSceneMelody/Rocket.wav", bgmParams, true, true));

    PlayParams clearParams;
    m_AudioList.emplace("clear", AudioConfig(L"SE/GameReaction/True2.wav", clearParams, false, false));

    RegesterReactionSE("clear");
    RegisterAudio();
}

void GameSceneRocket::Update(float tick)
{
    // デバッグ中
    return;
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

    GameSceneExe::Finalize();
}
