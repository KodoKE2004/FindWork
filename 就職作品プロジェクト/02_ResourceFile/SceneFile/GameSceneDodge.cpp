#include "GameSceneDodge.h"
#include "Game.h"
#include "Calculator.h"
#include "DebugUI.h"
#include <algorithm>
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

    const Calculator::Physics::VerticalMotionState kStoneVerticalMotion = []
        {
            Calculator::Physics::VerticalMotionState state{};
            state.mag = 120.0f;
            state.dt = 1.0f / 60.0f;
            state.groundY = -700.0f;
            return state;
        }();

    const Vector2 kBirdForce = {
        -1900.0f, 300.0f
    };


}

GameSceneDodge::GameSceneDodge(Camera& cam) : GameSceneExe(cam)
{
}

void GameSceneDodge::Initialize()
{
    DebugUI::TEXT_CurrentScene = "GameSceneDodge";

    // シーンに繋ぐ情報は基底初期化後の一番最初に設定
    m_RelationData.isClear = true;

    // リズムの定義
    RhythmBeatConst beatConfig{};
    auto& rhythmBeat = Game::GetRhythmBeat();
    beatConfig.Setup(Game::GetBgmBpm());
    rhythmBeat.Initialize(beatConfig, false, BASE_BEATS + 8);

    // ゲーム内の総拍数を参照するためリズム定義より後
    GameSceneExe::Initialize();

    auto& instance = Game::GetInstance();
    TextureManager* textureMar = instance; 

    m_Background = AddObject<Square>(instance.GetCamera());
    m_Background->SetName("m_Background");
    m_Background->SetScale(1280.0f, 720.0f, 1.0f);
    m_Background->SetTexture(textureMar->GetTexture("BackGround/GameSky.png"));

    m_Bird = AddObject<Bird>(instance.GetCamera());
    m_Bird->SetScale(50.0f,50.0f,1.0f);

    PlayParams fallParams;
    m_AudioList.emplace("fall", AudioConfig(L"SE/RockFall.wav", fallParams, false, false));

    PlayParams hitParams;
    m_AudioList.emplace("hit", AudioConfig(L"SE/GameReaction/StoneCollision.wav", hitParams, false, false));

    PlayParams bgmParams;
    m_AudioList.emplace("bgmDodge", AudioConfig(L"BGM/GameSceneMelody/Bird.wav", bgmParams, true, true));

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

    m_MySceneObjects.emplace_back(m_Bomber->GetRope());
    m_MySceneObjects.emplace_back(m_Bomber->GetNumber());

    PlaySE("bgmDodge", 0.2f);
}

void GameSceneDodge::Update(float tick)
{
    GameSceneExe::Update(tick);

    auto& instance = Game::GetInstance();
    
    m_StoneSpawnElapsed += tick;
    // while入る場合一度だけ
    int first = 0;
    while (m_StoneSpawnElapsed >= kStoneSpawnInterval)
    {
        if (first == 0)
        {
            
            first++;
        }
        
        m_StoneSpawnElapsed -= kStoneSpawnInterval;
        int createNum = 4 * (m_RelationData.stageCount / 6 + 1);
        
        for (int i = 0; i < createNum; ++i)
        {
            pShared<Stone> stone = AddObject<Stone>(instance.GetCamera());
            stone->Initialize();
            stone->SetScale(100.0f,100.0f,1.0f);
        }
    }

    auto stoneList = GetObjects<Stone>();
    

    for (const auto& stone : stoneList)
    {
        if (!stone) {
            continue;
        }
        stone->Update();
        if (!stone->IsActive()) {
            DeleteObject(stone);
        }

        if(m_Bird->IsAlive()) {

            // 当たり判定
            auto birdTransform  = m_Bird->GetTransform();
            auto stoneTransform = stone->GetTransform();
            bool isHit = Calculator::Collider2D::isHitCircleCircle(birdTransform, stoneTransform);
            if (isHit)
            {
                AudioManager* audioMgr = instance;
                Debug::Log("[[衝突]] Bird - Stone");
                m_Bird->DeAlive();
                Calculator::Physics::AddForce(m_Bird->GetMotionState().velocity, kBirdForce);
                m_ReactionActive = audioMgr->Create(m_AudioList.at("hit"));
                m_ReactionActive->Play(m_AudioList.at("hit").params);
            }
        }
    }

    if (m_ReactionActive) {
        if (m_ReactionActive->IsFinished() &&
            IsChangeMeasure()) {
            FastChange();
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

