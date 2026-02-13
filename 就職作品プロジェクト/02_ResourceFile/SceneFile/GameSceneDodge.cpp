#include "GameSceneDodge.h"
#include "Game.h"
#include "Calculator.h"
#include "DebugUI.h"
#include <random>
#include <array>
namespace
{
    constexpr float kStoneSpawnInterval = 0.5f;

    float StartPosX[10] = {
          0.0f,
        200.0f,
        400.0f,
        };
    std::array<size_t, 3> ShuffleButtonIndices()
    {
        static std::mt19937 engine{ std::random_device{}() };
        std::array<size_t, 3> indices{ 0, 1, 2 };
        std::shuffle(indices.begin(), indices.end(), engine);
        return indices;
    }
}

GameSceneDodge::GameSceneDodge(Camera& cam) : GameSceneExe(cam)
{
}

void GameSceneDodge::Initialize()
{
    DebugUI::TEXT_CurrentScene = "GameSceneJump";

    // シーンに繋ぐ情報は基底初期化後の一番最初に設定
    m_RelationData.isClear = true;

    // リズムの定義
    RhythmBeatConst beatConfig{};
    auto& rhythmBeat = Game::GetRhythmBeat();
    beatConfig.Setup(Game::GetBgmBpm(), 8, 4, 16, 4);
    rhythmBeat.Initialize(beatConfig, false, BASE_BEATS);

    // ゲーム内の総拍数を参照するためリズム定義より後
    GameSceneExe::Initialize();

    auto& instance = Game::GetInstance();
    TextureManager* textureMar = instance; 

    m_Background = AddObject<Square>(instance.GetCamera());
    m_Background->SetName("m_Background");
    m_Background->SetScale(1280.0f, 720.0f, 1.0f);
    m_Background->SetTexture(textureMar->GetTexture("Plane.png"));

    m_Bird = AddObject<Bird>(instance.GetCamera());
    m_Bird->SetScale(50.0f,50.0f,1.0f);

    PlayParams fallParams;
    m_AudioList.emplace("fall", AudioConfig(L"SE/RockFall.wav", fallParams, false, false));

    AudioManager* audioMgr = instance;
    if (audioMgr)
    {
        for (const auto& [key, config] : m_AudioList)
        {
            if (!audioMgr->Add(key, config.filePath)) {
                continue;
            }
            if (config.autoPlay)
            {
                auto params = config.params;
                if (config.loop)
                {
                    params.loop.loopCount = XAUDIO2_LOOP_INFINITE;
                }
            }
        }
    }

    m_Bomber = AddObject<Bomber>(instance.GetCamera());
    m_Bomber->SetName("m_TimeGauge");

}

void GameSceneDodge::Update(float tick)
{
    GameSceneExe::Update(tick);

    auto& instance = Game::GetInstance();
    
    // 拍が更新された場合
    m_StoneSpawnElapsed += tick;
    while (m_StoneSpawnElapsed >= kStoneSpawnInterval)
    {
        m_StoneSpawnElapsed -= kStoneSpawnInterval;
        int createNum = 8 * (m_RelationData.stageCount / 6 + 1);
        PlaySE("fall", 0.5f);
        for (int i = 0; i < createNum; ++i)
        {
            std::shared_ptr<Stone> stone = std::make_shared<Stone>(instance.GetCamera());
            stone->Initialize();
            m_StoneList.emplace_back(stone);
        }
    }

    for (auto stone : m_StoneList)
    {
        if (!stone->IsActive()) {
            DeleteObject(stone);
        }
    }


    if (IsChange())
    {
        ChangeScene();
    }




}

void GameSceneDodge::Draw()
{
    Scene::Draw();
}

void GameSceneDodge::Finalize()
{
    GameSceneExe::Finalize();
}

