#include "GameSceneShot.h"
#include "Game.h"
#include "Calculator.h"
#include "Enemy.h"

#include <random>

using namespace Calculator::Collider2D;

namespace
{
    constexpr int kSpawnRetryMax = 32;
    constexpr float kCenterAvoidHalfWidth = 100.0f;
    constexpr float kMinSpawnY = 50.0f;

    struct SpawnRange
    {
        float minX = 0.0f;
        float maxX = 0.0f;
        float minY = 0.0f;
        float maxY = 0.0f;
    };

    bool IsFarEnoughFromOthers(const NVector3& candidate, const std::vector<NVector3>& spawnedPositions, float minSpacing)
    {
        const float minSpacingSquared = minSpacing * minSpacing;
        for (const auto& spawned : spawnedPositions)
        {
            const float dx = candidate.x - spawned.x;
            const float dy = candidate.y - spawned.y;
            const float distanceSquared = (dx * dx) + (dy * dy);
            if (distanceSquared < minSpacingSquared)
            {
                return false;
            }
        }
        return true;
    }

    SpawnRange BuildSpawnRange(const NVector3& enemyScale)
    {
        // 画面の中心を避けるため、敵の半分の幅を考慮してスポーン範囲を計算
        // 画面内に完全に収まるように、敵の半分の幅と高さを考慮して範囲を設定
        // ただし仕様上当たらない箇所が存在するため幅はさらに狭める
        const float halfScreenWidth  = static_cast<float>(Application::GetWidth())  * 0.5f - 200.0f;
        const float halfScreenHeight = static_cast<float>(Application::GetHeight()) * 0.5f;
        const float halfW = enemyScale.x * 0.5f;
        const float halfH = enemyScale.y * 0.5f;

        SpawnRange range;
        range.minX = -halfScreenWidth + halfW;
        range.maxX =  halfScreenWidth - halfW;
        range.minY = max(-halfScreenHeight + halfH, kMinSpawnY);
        range.maxY = halfScreenHeight - halfH;
        return range;
    }

    NVector3 GenerateSpawnCandidate(std::mt19937& engine, const SpawnRange& range, bool avoidCenter)
    {
        std::uniform_real_distribution<float> distY(range.minY, range.maxY);
        float posX = 0.0f;

        if (avoidCenter)
        {
            std::uniform_int_distribution<int> distSide(0, 1);
            if (distSide(engine) == 0)
            {
                std::uniform_real_distribution<float> distX(range.minX, -kCenterAvoidHalfWidth);
                posX = distX(engine);
            }
            else
            {
                std::uniform_real_distribution<float> distX(kCenterAvoidHalfWidth, range.maxX);
                posX = distX(engine);
            }
        }
        else
        {
            std::uniform_real_distribution<float> distX(range.minX, range.maxX);
            posX = distX(engine);
        }

        return NVector3(posX, distY(engine), 1.0f);
    }
}

GameSceneShot::GameSceneShot(Camera& cam) : GameSceneExe(cam)
{
}

void GameSceneShot::Initialize()
{
#ifdef _DEBUG
    DebugUI::TEXT_CurrentScene = "GameSceneShot";
#endif 
    auto& instance = Game::GetInstance();
    TextureManager* textureMgr = instance;

    // シーンに繋ぐ情報は基底初期化後の一番最初に設定
    m_RelationData.isClear = false;

    m_CreateBulletTime = 0.05f;

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
    m_Player->SetName("m_Player");
    m_Player->SetTexture(textureMgr->GetTexture("GameScene/GamePlane.png"));
    m_Player->SetPos  (  0.0f,-200.0f, 0.0f);
    m_Player->SetScale(300.0f, 200.0f, 0.0f);

    m_OperatorBar = AddObject<Square>(instance.GetCamera());
    m_OperatorBar->SetName("m_OperatorBar");
    m_OperatorBar->SetTexture(textureMgr->GetTexture("GameScene/OperationBar.png"));
    m_OperatorBar->SetPos  (  0.0f, - 270.0f, 0.0f);
    m_OperatorBar->SetScale(900.0f,   100.0f, 1.0f);

    m_PlaneHandle = AddObject<DragController>(instance.GetCamera());
    m_PlaneHandle->SetName("m_PlaneHandle");
    m_PlaneHandle->SetDirection(MOVE_RIGHT);
    m_PlaneHandle->SetTexture(textureMgr->GetTexture("GameScene/PlaneHandle.png"));
    m_PlaneHandle->SetPos  ( 0.0f, -270.0f, 1.0f);
    m_PlaneHandle->SetScale(50.0f,   50.0f, 1.0f);
    m_PlaneHandle->SetLimitRange({ m_DragLimitLine, 1000.0f, 1000.0f});

    //m_Player->SetScale();
    int difficult = m_RelationData.stageCount / 4;
    if (difficult >= 4) { difficult = 3; }

    const int spawnCount = difficult + 1;
    std::vector<NVector3> spawnedEnemyPositions;
    spawnedEnemyPositions.reserve(spawnCount);
    static std::mt19937 spawnEngine{ std::random_device{}() };

    for (int i = 0; i < spawnCount; ++i)
    {
        auto enemy = AddObject<Enemy>(instance.GetCamera());
        enemy->SetName("m_Enemy");

        const SpawnRange spawnRange = BuildSpawnRange(enemy->GetScale());
        const float minSpacing = enemy->GetScale().x;
        const bool avoidCenter = (i == 0);

        NVector3 spawnPos = GenerateSpawnCandidate(spawnEngine, spawnRange, avoidCenter);
        for (int retry = 0; retry < kSpawnRetryMax; ++retry)
        {
            spawnPos = GenerateSpawnCandidate(spawnEngine, spawnRange, avoidCenter);
            if (IsFarEnoughFromOthers(spawnPos, spawnedEnemyPositions, minSpacing))
            {
                break;
            }
        }

        enemy->SetPos(spawnPos);
        spawnedEnemyPositions.emplace_back(spawnPos);

    }
    m_Bomber = AddObject<Bomber>(instance.GetCamera());
    m_Bomber->SetName("m_TimeGauge");
    m_MySceneObjects.emplace_back(m_Bomber->GetRope());
    m_MySceneObjects.emplace_back(m_Bomber->GetNumber());
    
    PlayParams shotParams;
    m_AudioList.emplace("shot", AudioConfig(L"SE/BulletShot.wav", shotParams, false, false));

    PlayParams hitParams;
    m_AudioList.emplace("hit", AudioConfig(L"SE/BulletHit.wav" , hitParams, false, false));
   
    PlayParams bgmParams;
    m_AudioList.emplace("bgmShot", AudioConfig(L"BGM/GameSceneMelody/Shooting.wav" , bgmParams, false, false));

    PlayParams exploParams;
    m_AudioList.emplace("explosion", AudioConfig(L"SE/GameReaction/Explosion.wav" , exploParams, false, false));

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


    RegesterReactionSE("explosion");
    PlaySE("bgmShot", 0.4f);
}

void GameSceneShot::Update(float tick)
{
    // Skydomeの回転
    m_Skydome->Spin(0.0f, -4.0f, 0.0f);


    GameSceneExe::Update(tick);
    
    // ハンドルの位置に合わせて、プレイヤーも移動させる
    NVector3 planePos = {
        m_PlaneHandle->GetPos().x - 7.0f,
        m_Player->GetPos().y,
        m_Player->GetPos().z,
    };
    m_Player->SetPos(planePos);

    m_CreateBulletElapsed += tick;
    if (m_CreateBulletTime <= m_CreateBulletElapsed)
    {
        CreateBullet();
        m_CreateBulletElapsed = 0.0f;
    }

    // 弾と敵の当たり判定
    auto bullets = GetObjects<Bullet>();
    auto enemys = GetObjects<Enemy>();
    for (auto enemy : enemys)
    {
        if (enemy->IsDeath()) {
            if (enemy->IsLifeSpan()) {
                DeleteObject(enemy);
            }
            continue;
        }
        for (auto bullet : bullets)
        {
            if (!bullet->IsAlive()) {
                continue;
            }
            auto enemyTransform  = enemy->GetTransform();
            auto bulletTransform = bullet->GetTransform();
            bool isCollision = Calculator::Collider2D::isHitCircleCircle(enemyTransform, bulletTransform);

            if (bullet->IsAlive() && isCollision)
            {
                PlaySE("hit", 0.5f);
                bullet->DeAlive();
                enemy->Damage(1);
                if (enemy->IsDeath()) {
                    m_ReactionActive->Play(m_AudioList.at("explosion").params);
                }
                break;
            }
        }   
    }
    if (IsAllDeathEnemy(enemys)) {
        // SceneExeで早めにクリアをした場合も想定
        StageClear();
        bool isFinished = false;
        if (m_ReactionActive) {
            isFinished = m_ReactionActive->IsFinished();
        }
        if (isFinished && IsChangeMeasure()) {
            FastChange();
        }
    }
    auto scene = Game::GetInstance().GetCurrentScene();
    for (auto bullet : bullets) {
        if (!bullet->IsAlive()) {
            scene->DeleteObject(bullet);
        }
    }

    if (!m_RelationData.isClear) 
    {

    }

    if (IsChange())
    {
        ChangeScene();
    }
}

void GameSceneShot::Draw()
{
    Scene::Draw();
}

void GameSceneShot::Finalize()
{
    GameSceneExe::Finalize();
}

void GameSceneShot::CreateBullet()
{
    auto& instance = Game::GetInstance();
    TextureManager* textureMgr = instance;

    pShared<Bullet> bullet = AddObject<Bullet>(instance.GetCamera());
    bullet->SetTexture(textureMgr->GetTexture("GameScene/Bullet.png"));
    Vector3 pos = {
        m_Player->GetPos().x +  8.5f,
        m_Player->GetPos().y + 40.0f,
        m_Player->GetPos().z,
    };
    bullet->SetName("m_Bullet");
    bullet->Alive();
    bullet->SetPos(pos);
    bullet->SetScale(25.0f,50.0f,0.0f);
    bullet->Shoot (pos, Vector3(0.0f,1.0f,0.0f));
    PlaySE("shot", 0.2f);
}
