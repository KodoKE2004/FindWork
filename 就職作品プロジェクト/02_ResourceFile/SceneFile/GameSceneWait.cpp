#include "GameSceneWait.h"
#include "Game.h"
#include "DebugUI.h"
#include "SceneList.h"

#include <array>
#include <vector>
#include <random>
#include <cmath>

bool GameSceneWait::s_HasFirstGameSceneWaitInitialized = false;
GAME_PHASE GameSceneWait::s_CurrentGamePhase = GAME_PHASE::START;

namespace
{
    struct StageEntry
    {
        SCENE_NO scene;
        void (*transition)();
    };

    template<class T>
    void PushGameStage()
    {
        ChangeScenePush<T>(WaitToGame);
    }

    const size_t GAME_EXE_NUM = static_cast<size_t>(SCENE_NO::EXE_NUM);

    const std::array<StageEntry, GAME_EXE_NUM> kStageEntries = { {
        { SCENE_NO::GAME_SLICE, &PushGameStage<GameSceneSlice> },
        { SCENE_NO::GAME_JUMP , &PushGameStage<GameSceneJump>  },
        { SCENE_NO::GAME_CRUSH, &PushGameStage<GameSceneCrush> },
        { SCENE_NO::GAME_TEXT , &PushGameStage<GameSceneText> },

    } };

    std::vector<SCENE_NO> BuildStageCandidates(SCENE_NO excludeScene)
    {
        std::vector<SCENE_NO> candidates;
        candidates.reserve(kStageEntries.size());

        for (const auto& entry : kStageEntries)
        {
            if (entry.scene != excludeScene)
            {
                candidates.emplace_back(entry.scene);
            }
        }

        if (candidates.empty())
        {
            for (const auto& entry : kStageEntries)
            {
                candidates.emplace_back(entry.scene);
            }
        }

        return candidates;
    }

    SCENE_NO SelectRandomStage(std::mt19937_64& randomEngine, SCENE_NO excludeScene)
    {
        const std::vector<SCENE_NO> candidates = BuildStageCandidates(excludeScene);
        std::uniform_int_distribution<std::size_t> dist(0, candidates.size() - 1);
        return candidates[dist(randomEngine)];
    }

    const StageEntry* FindStageEntry(SCENE_NO scene)
    {
        for (const auto& entry : kStageEntries)
        {
            if (entry.scene == scene)
            {
                return &entry;
            }
        }
        return nullptr;
    }

    const char* kStageTheme[GAME_EXE_NUM] = {
        "Theme/Avoid.png",
        "Theme/Hit.png",
        "Theme/Slice.png",
        "Theme/Convey.png"
    };

    const NVector3 kThemeScale[GAME_EXE_NUM] = {
        NVector3( 550.0f, 200.0f, 1.0f),
        NVector3( 546.0f, 223.0f, 1.0f),
        NVector3( 417.0f, 217.0f, 1.0f),
        NVector3( 554.0f, 198.0f, 1.0f),
    };

    constexpr float kStageTransitionDelay = 1.0f;

    constexpr uint32_t kLifeParticleCount = 12u;
    constexpr float    kLifeParticleSpeed = 400.0f;
    constexpr float    kLifeParticleLifeSec = 0.8f;
}

void GameSceneWait::Initialize()
{
    DebugUI::TEXT_CurrentScene = "GameSceneWait";

    // 最初の一度だけ or 指定したタイミングのみフラグを立てる
    m_IsFirstInitialized = !s_HasFirstGameSceneWaitInitialized;
    s_HasFirstGameSceneWaitInitialized = true;

    // 引き渡しデータのシーンの整理
    m_RelationData.oldScene      = m_RelationData.previousScene;
    m_RelationData.previousScene = SCENE_NO::GAME_WAIT;    
    m_RelationData.texture       = nullptr;

    if (m_RelationData.isClear) {
        Debug::Log("=====  ステージ成功  =====");
    }
    else {
        Debug::Log("=====  ステージ失敗  =====");
    }

    RhythmBeatConst beatConfig{};
    beatConfig.Setup(Game::GetBgmBpm(), 4, 1);

    // 難易度アップ処理 
    ++m_RelationData.stageCount;
    // 難易度 0 ~
    if (m_RelationData.stageCount % 8 == 0) {
        int difficulty = m_RelationData.stageCount / 8;
        m_Difficulty = difficulty;
        Debug::Log("[[検出]] 難易度アップ");
    }
    // スピード
    else if (m_RelationData.stageCount % 4 == 0) {
        beatConfig.Setup(Game::GetBgmBpm() + (m_RelationData.stageCount / 4) * 10.0f, 4, 1);
        Debug::Log("[[検出]] スピードアップ");

    }

    Game::SetBgmBpm(beatConfig.m_Bpm);

    m_TimerList.clear();
    SetTimer(&m_Tick);
    SetTimer(&m_DecrementLife.timer);

    m_RelationData.rhythmBeat.Initialize(beatConfig);

    m_BeatTimer.Initialize(8);

    m_IsFirstInitialized = true;

    auto& instance = Game::GetInstance();
    TextureManager* textureMgr = instance.GetInstance();

    m_LifeParticleEmitter = std::make_shared<ParticleEmitter>(instance.GetCamera());

    // スカイドーム初期化
    // Skydome初期化 
    m_Skydome = instance.AddObject<Skydome>();
    m_Skydome->SetName("m_Skydome");
    m_Skydome->SetSkyDomeMode(true);
    m_Skydome->SetTexture(textureMgr->GetTexture("SkydomeSpace.png"));
    m_Skydome->SetRadius(500.0f);
    m_MySceneObjects.emplace_back(m_Skydome);


    // ライフの数だけハートの生成
    const float lifePosX = - 200.0f;
    const float lifePosY = - 100.0f;

    m_ShouldTransitionToStage = false;
    m_wasDecrementLife        = false;

    m_LifeGame.clear();
    m_LifeCount = m_RelationData.gameLife;
    // ライフオブジェクトの生成
    for(uint32_t i = 0; i < m_LifeCount; ++i)
    {
        const float distance = 130.0f;

        std::shared_ptr<Square> life = instance.AddObject<Square>(instance.GetCamera());
        life->SetTexture(textureMgr->GetTexture("DestroyBullet.png"));
        life->SetPos(lifePosX + ( i * distance), lifePosY, 1.0f);
        life->SetName("m_life");
        life->SetScale(100.0f, 100.0f, 1.0f);
        life->SetShader("VS_Alpha","PS_Alpha");
        life->SetColor(1.0f, 1.0f, 1.0f, 1.0f);

        m_MySceneObjects.emplace_back(life);
        m_LifeGame.emplace_back(life);
    }

    const float initialTilt = m_IsLifeTiltPositive ? 30.0f : -30.0f;
    for (auto life : m_LifeGame)
    {
        if (life)
        {
            life->SetRotate(life->GetRotate().x, life->GetRotate().y, initialTilt);
        }
    }

    // ステージ乱数選択処理   
    PrepareNextStage();

    // ステージ用のお題を作成
    m_Theme = instance.GetTheme();
    if (m_Theme)
    {
        m_Theme->SetName("m_Theme");
        m_Theme->SetActive(false);

        size_t path = static_cast<size_t>(m_RelationData.nextScene);
        m_Theme->SetTexture(textureMgr->GetTexture(kStageTheme[path]));
        m_Theme->SetScaleBase(kThemeScale[path]);
        m_Theme->SetPos(0.0f,0.0f,0.0f);
    }

    Debug::Log("===== クリアステージ数 : " + std::to_string(m_RelationData.stageCount) + " =====");
}

void GameSceneWait::Update(float tick)
{   
    auto& instance = Game::GetInstance();

    // リズムを取る
    // ライフをリズムに合わせて回転させる
    int advancedTicks = m_RelationData.rhythmBeat.Update(tick);
    if (advancedTicks > 0)
    {
        const int currentBeatIndex = m_RelationData.rhythmBeat.GetBeatIndex();

        m_BeatTimer.Advance(currentBeatIndex);
        if (m_BeatTimer.IsBeatZero())
        {
            m_ShouldTransitionToStage = true;
        }
        // 残り一拍のタイミングでお題提示処理開始
        else if (m_BeatTimer.GetRestBeats() == 1)
        {
            m_Theme->SetActive(true);
        }
        if (advancedTicks % 2 == 1)
        {
            m_IsLifeTiltPositive = !m_IsLifeTiltPositive;
        }
        const float tiltAngle = m_IsLifeTiltPositive ? 30.0f : -30.0f;
        for (auto life : m_LifeGame)
        {
            if (life)
            {
                life->SetRotate(0.0f, 0.0f, tiltAngle);
            }
        }
    }

    if (m_ShouldTransitionToStage)
    {
        StartNextStageTransition();
    }


    // ライフ減少処理
    if ( m_DecrementLife.IsTimeUp() &&
        !m_wasDecrementLife         &&
        !m_RelationData.isClear)
    {
        // ライフが減る演出
        

        // ライフを減らす
        m_RelationData.gameLife -= 1u;
        DecrementLife();

        m_wasDecrementLife = true;
    }
    if (m_LifeParticleEmitter)
    {
        m_LifeParticleEmitter->Update(tick);
    }
    // タイマー更新処理
    CountTimer(tick);

    // デバッグ用　終わったら消す予定のreturn
    // return ;
    if (m_RelationData.gameLife == 0u)
    {
        // ライフが0になったらリザルトシーンへ
        ChangeScenePush<ResultScene>(WaitToResult);
    }
}

void GameSceneWait::Finalize()
{
    auto& instance = Game::GetInstance();

    // このシーンのオブジェクトを削除する
    for (auto o : m_MySceneObjects) {
        instance.DeleteObject(o);
    }
    m_MySceneObjects.clear();
    // オーディオの停止
    if (AudioManager* audioManager = instance)
    {
        for (const auto& [key, config] : m_AudioList)
        {
            audioManager->StopAllByName(key);
        }
    }
}

// 次のステージ選択とシーン遷移処理
void GameSceneWait::StartNextStageTransition()
{
    ApplyBeatDuration(WaitToGame, m_RelationData);

    // シーン遷移処理
    switch (m_RelationData.nextScene)
    {
    case SCENE_NO::GAME_SLICE: ChangeScenePush<GameSceneSlice>(WaitToGame); break;
    case SCENE_NO::GAME_JUMP : ChangeScenePush<GameSceneJump> (WaitToGame); break;
    case SCENE_NO::GAME_CRUSH: ChangeScenePush<GameSceneCrush>(WaitToGame); break;
    case SCENE_NO::GAME_TEXT : ChangeScenePush<GameSceneText> (WaitToGame); break;
    default: return;
    }
}

void GameSceneWait::DecrementLife()
{
    auto& instance = Game::GetInstance();
    if (!m_LifeGame.empty())
    instance.DeleteObject(m_LifeGame.back());
    m_LifeGame.pop_back();
    --m_LifeCount;
}


void GameSceneWait::PrepareNextStage()
{
    // ステージのインデックスを格納
    SCENE_NO nextScene = StageSelectAllRandom();
    nextScene = SCENE_NO::GAME_TEXT;
    // 次のシーンRelationDataに格納
    m_RelationData.nextScene = nextScene;
}

// 全ステージから選ぶ時の乱数を取得する関数

SCENE_NO GameSceneWait::StageSelectAllRandom()
{
    return SelectRandomStage(m_RandomEngine, m_RelationData.oldScene);
}