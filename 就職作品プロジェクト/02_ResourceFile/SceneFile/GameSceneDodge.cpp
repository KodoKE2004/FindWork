#include "GameSceneDodge.h"
#include "Game.h"
#include "Calculator.h"
#include "DebugUI.h"
#include <algorithm>
#include <random>
#include <array>
#include <cmath>
namespace
{
    constexpr float kStoneSpawnInterval = 0.5f;
    constexpr int kStoneSpawnMaxRetry = 32;
    constexpr float kStoneMinPosY = 600.0f;
    constexpr std::array<float, 13> StartPosX = {
       -600.0f,
       -500.0f,
       -400.0f,
       -300.0f,
       -200.0f,
       -100.0f,
        0.0f,
        100.0f,
        200.0f,
        300.0f,
        400.0f,
        500.0f,
        600.0f
    };

    std::array<size_t, StartPosX.size()> ShuffleLaneIndices()
    {
        static std::mt19937 engine{ std::random_device{}() };
        std::array<size_t, StartPosX.size()> indices{};
        for (size_t i = 0; i < StartPosX.size(); ++i)
        {
            indices[i] = i;
        }
        std::shuffle(indices.begin(), indices.end(), engine);
        return indices;
    }

    bool IsOverlappedAABB(const NVector3& candidate,
        float candidateHalfW,
        float candidateHalfH,
        const vector<pShared<Stone>>& existingStones)
    {
        for (const auto& otherStone : existingStones)
        {
            if (!otherStone || !otherStone->IsActive())
            {
                continue;
            }

            const auto otherPos = otherStone->GetPos();
            const auto otherScale = otherStone->GetScale();
            const float otherHalfW = std::abs(otherScale.x) * 0.5f;
            const float otherHalfH = std::abs(otherScale.y) * 0.5f;

            const float dx = std::abs(candidate.x - otherPos.x);
            const float dy = std::abs(candidate.y - otherPos.y);

            if (dx < (candidateHalfW + otherHalfW) &&
                dy < (candidateHalfH + otherHalfH))
            {
                return true;
            }
        }

        return false;
    }

    NVector3 ResolveStoneSpawnPos(const vector<pShared<Stone>>& existingStones,
        float stoneHalfW,
        float stoneHalfH)
    {
        static std::mt19937 engine{ std::random_device{}() };

        const float screenHalfW = static_cast<float>(Application::GetWidth())  * 0.5f;
        const float screenHalfH = static_cast<float>(Application::GetHeight()) * 0.5f + 300.0f;

        const float screenLeft = -screenHalfW;
        const float screenRight = screenHalfW;
        const float screenBottom = -screenHalfH;
        const float screenTop = screenHalfH;

        const float minX = screenLeft + stoneHalfW;
        const float maxX = screenRight - stoneHalfW;
        const float minY = max(kStoneMinPosY, screenBottom + stoneHalfH);
        const float maxY = screenTop - stoneHalfH;

        const float spawnY = (minY <= maxY) ? maxY : minY;
        NVector3 fallbackCandidate(0.0f, spawnY, 0.0f);

        for (int attempt = 0; attempt < kStoneSpawnMaxRetry; ++attempt)
        {
            const auto laneIndices = ShuffleLaneIndices();

            for (const auto laneIndex : laneIndices)
            {
                const float laneX = std::clamp(StartPosX[laneIndex], minX, maxX);
                const float laneY = (minY <= maxY)
                    ? std::uniform_real_distribution<float>(minY, maxY)(engine)
                    : spawnY;
                const NVector3 candidate(laneX, laneY, 0.0f);
                fallbackCandidate = candidate;

                if (!IsOverlappedAABB(candidate, stoneHalfW, stoneHalfH, existingStones))
                {
                    return candidate;
                }
            }
        }

        return fallbackCandidate;
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
    StageClear();

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

    m_Stone = std::make_shared<Stone>(instance.GetCamera());
    m_Stone->Initialize();
    PlayParams fallParams;
    m_AudioList.emplace("fall", AudioConfig(L"SE/RockFall.wav", fallParams, false, false));

    PlayParams hitParams;
    m_AudioList.emplace("hit", AudioConfig(L"SE/GameReaction/StoneCollision.wav", hitParams, false, false));

    PlayParams bgmParams;
    m_AudioList.emplace("bgmDodge", AudioConfig(L"BGM/GameSceneMelody/Bird.wav", bgmParams, true, true));

    RegisterAudio();

    m_Bomber = AddObject<Bomber>(instance.GetCamera());
    m_Bomber->SetName("m_TimeGauge");

    m_MySceneObjects.emplace_back(m_Bomber->GetRope());
    m_MySceneObjects.emplace_back(m_Bomber->GetNumber());

    PlaySE("bgmDodge", 0.2f);

    RegesterReactionSE("fall");
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
            PlaySE("fall",0.2f);
        }
        
        m_StoneSpawnElapsed -= kStoneSpawnInterval;
        int createNum = 4 * (m_RelationData.stageCount / 8 + 1);
        if (createNum >= 12) {
            createNum = 12;
        }
        for (int i = 0; i < createNum; ++i)
        {
            auto existingStones = GetObjects<Stone>();

            pShared<Stone> stone = std::make_shared<Stone>(instance.GetCamera());
            stone->Initialize();
            stone->SetScale(100.0f, 100.0f, 1.0f);

            const auto stoneScale = stone->GetScale();
            const float stoneHalfW = std::abs(stoneScale.x) * 0.5f;
            const float stoneHalfH = std::abs(stoneScale.y) * 0.5f;
            const auto spawnPos = ResolveStoneSpawnPos(existingStones, stoneHalfW, stoneHalfH);
            stone->SetPos(spawnPos.x, spawnPos.y, spawnPos.z);
            m_StoneList.emplace_back(stone);
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

                StageFail();
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
    m_Stone->DrawInstanced(m_StoneList);
}

void GameSceneDodge::Finalize()
{
    // このシーンのオブジェクトを削除する
    for (auto o : m_MySceneObjects) {
        DeleteObject(o);
    }
    m_MySceneObjects.clear();
    GameSceneExe::Finalize();
}

