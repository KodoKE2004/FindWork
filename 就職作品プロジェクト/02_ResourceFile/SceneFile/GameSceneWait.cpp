#include "GameSceneWait.h"
#include "Game.h"
#include "DebugUI.h"
#include "SceneList.h"
#include "TransitionBase.h"

#include <array>
#include <vector>
#include <random>
#include <cmath>

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
        { SCENE_NO::GAME_DODGE, &PushGameStage<GameSceneDodge>  },
        { SCENE_NO::GAME_SHOT , &PushGameStage<GameSceneShot> },
        { SCENE_NO::GAME_TEXT , &PushGameStage<GameSceneText> },
    } };

    using StageList = std::vector<SCENE_NO>;

    StageList BuildStageCandidates_Exclude(SCENE_NO excludeScene)
    {
        StageList candidates;
        candidates.reserve(kStageEntries.size());

        for (const auto& entry : kStageEntries)
        {
            if (entry.scene != excludeScene)
                candidates.emplace_back(entry.scene);
        }

        // 保険：全部除外されてしまったら全候補に戻す
        if (candidates.empty())
        {
            for (const auto& entry : kStageEntries)
                candidates.emplace_back(entry.scene);
        }

        return candidates;
    }

    StageList BuildStageCandidates_All()
    {
        StageList candidates;
        candidates.reserve(kStageEntries.size());
        for (const auto& entry : kStageEntries)
            candidates.emplace_back(entry.scene);
        return candidates;
    }

    SCENE_NO SelectRandomFrom(std::mt19937_64& eng, const StageList& candidates)
    {
        std::uniform_int_distribution<std::size_t> dist(0, candidates.size() - 1);
        return candidates[dist(eng)];
    }

    SCENE_NO SelectRandomStage_All(std::mt19937_64& eng)
    {
        const StageList candidates = BuildStageCandidates_All();
        return SelectRandomFrom(eng, candidates);
    }

    SCENE_NO SelectRandomStage_Exclude(std::mt19937_64& eng, SCENE_NO excludeScene)
    {
        const StageList candidates = BuildStageCandidates_Exclude(excludeScene);
        return SelectRandomFrom(eng, candidates);
    }

    const char* kStageTheme[GAME_EXE_NUM] = {
        "Theme/Avoid.png",
        "Theme/KO.png",
        "Theme/Convey.png"
    };

    const NVector3 kThemeScale[GAME_EXE_NUM] = {
        NVector3( 546.0f, 223.0f, 1.0f),
        NVector3( 557.0f, 217.0f, 1.0f),
        NVector3( 554.0f, 198.0f, 1.0f),
    };

    constexpr float kStageTransitionDelay = 1.0f;

    constexpr uint32_t kLifeParticleCount = 12u;
    constexpr float    kLifeParticleSpeed = 400.0f;
    constexpr float    kLifeParticleLifeSec = 0.8f;
}

GameSceneWait::GameSceneWait(Camera& cam) : Scene(cam)
{
}

void GameSceneWait::Initialize()
{
    DebugUI::TEXT_CurrentScene = "GameSceneWait";

    auto& instance = Game::GetInstance();
    TextureManager* textureMgr = instance.GetInstance();

    // 最初の一度だけ or 指定したタイミングのみフラグを立てる
    m_IsFirstInitialized != instance.HasFirstGameSceneWaitInitialized();
    instance.SetHasFirstGameSceneWaitInitialized(true);

    // 引き渡しデータのシーンの整理
    m_RelationData.ClearTransitionTexture();

    if (m_RelationData.isClear) {
        Debug::Log("=====  ステージ成功  =====");
    }
    else {
        Debug::Log("=====  ステージ失敗  =====");
    }

    RhythmBeatConst beatConfig{};
    auto& rhythmBeat = Game::GetRhythmBeat();

    beatConfig.Setup(Game::GetBgmBpm());
    rhythmBeat.Initialize(beatConfig, false, 32);

    // 難易度アップ処理 
    ++m_RelationData.stageCount;

    const int difficultyStageInterval = Game::GetDifficultyStageInterval();
    const float baseBpmIncreasePerDifficulty = Game::GetBaseBpmIncreasePerDifficulty();
    const int speedUpStageInterval = Game::GetSpeedUpStageInterval();
    const float speedUpBpmIncrease = Game::GetSpeedUpBpmIncrease();

    // 難易度 0 ~
    if (m_RelationData.stageCount % difficultyStageInterval == 0) {
        int difficulty = m_RelationData.stageCount / difficultyStageInterval;

        const float upBpm = difficulty * baseBpmIncreasePerDifficulty;
        if (upBpm > 0.0f)
        {
            beatConfig.Setup(Game::GetBgmBpm());
        }
        Debug::Log("[[検出]] 難易度アップ");
    }
    // スピード
    else if (m_RelationData.stageCount % speedUpStageInterval == 0) {
        float upBpm = (static_cast<float>(m_RelationData.stageCount) / speedUpStageInterval) * speedUpBpmIncrease;
        beatConfig.Setup(Game::GetBgmBpm() + upBpm);
        Debug::Log("[[検出]] スピードアップ");

    }

    Game::SetBgmBpm(beatConfig.m_Bpm);

    m_TimerList.clear();
    SetTimer(&m_Tick);
    SetTimer(&m_DecrementLife.timer);
    m_WasPlayBGM         = false;
    m_QuarterAdvance     = 0;


    m_LifeParticleEmitter = std::make_shared<ParticleEmitter>(instance.GetCamera());

    // スカイドーム初期化
    m_Skydome = AddObject<Skydome>(instance.GetCamera());
    m_Skydome->SetName("m_Skydome");
    m_Skydome->SetSkyDomeMode(true);
    m_Skydome->SetTexture(textureMgr->GetTexture("SkydomeSpace.png"));
    m_Skydome->SetRadius(500.0f);

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

        pShared<Square> life = AddObject<Square>(instance.GetCamera());
        life->SetTexture(textureMgr->GetTexture("GameScene/DestroyBullet.png"));
        life->SetPos(lifePosX + ( i * distance), lifePosY, 1.0f);
        life->SetName("m_life");
        life->SetScale(100.0f, 100.0f, 1.0f);
        life->SetShader("VS_Alpha","PS_Alpha");
        life->SetColor(1.0f, 1.0f, 1.0f, 1.0f);

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
    auto& rhythmBeat = Game::GetRhythmBeat();
    // リズムを取る    
    m_QuarterAdvance = rhythmBeat.GetBeatElapsed() / 2;
    rhythmBeat.Update(tick);
    int elapsedBeat  = rhythmBeat.GetBeatElapsed() / 2;
    int restBeat     = rhythmBeat.GetBeatRest();
    // ライフをリズムに合わせて回転させる
    for (int i = m_QuarterAdvance; i < elapsedBeat; ++i)
    {
        // 1拍目のタイミングでBGM再生
        if (!m_WasPlayBGM)
        {
            Game::PlayBgm();
            m_WasPlayBGM = true;
        }
        
        // 残り一拍のタイミングでステージ遷移フラグを立てる
        if (restBeat <=  5)
        {
            m_Theme->SetActive(true);
        }

        elapsedBeat;
        if (elapsedBeat % 2 == 1)
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

    if (rhythmBeat.GetBeatElapsed() >= rhythmBeat.GetBeatTotal() - 3)
    {
        m_ShouldTransitionToStage = true;
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
    if (m_RelationData.gameLife == 0u)
    {
        // ライフが0になったらリザルトシーンへ
        Game::SetIsTickCount(false);
        
        ChangeScenePush<ResultScene>(WaitToResult);
    }
}

void GameSceneWait::Draw()
{
    Scene::Draw();
}

void GameSceneWait::Finalize()
{
    auto& instance = Game::GetInstance();

    // このシーンのオブジェクトを削除する
    for (auto o : m_MySceneObjects) {
        DeleteObject(o);
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
    Game::SetIsTickCount(true);
    RhythmBeat& rhythmBeat = Game::GetRhythmBeat();
    WaitToGame.duration = rhythmBeat.GetOneBeat();
    rhythmBeat.TickCount(WaitToGame.duration);

    // シーン遷移処理
    switch (m_RelationData.nextScene)
    {
    case SCENE_NO::GAME_DODGE: ChangeScenePush<GameSceneDodge>(WaitToGame); break;
    case SCENE_NO::GAME_SHOT : ChangeScenePush<GameSceneShot> (WaitToGame); break;
    case SCENE_NO::GAME_TEXT : ChangeScenePush<GameSceneText> (WaitToGame); break;
    default: return;
    }
}

void GameSceneWait::DecrementLife()
{
    if (!m_LifeGame.empty())
    DeleteObject(m_LifeGame.back());
    m_LifeGame.pop_back();
    --m_LifeCount;
}


void GameSceneWait::PrepareNextStage()
{
    // 初期値は保険（何でも良いが未初期化禁止）
    SCENE_NO nextScene = SCENE_NO::GAME_DODGE;

    if (m_IsFirstInitialized)
    {
        // 初回：全候補から
        nextScene = SelectRandomStage_All(m_RandomEngine);
    }
    else
    {
        // 2回目以降：直前と同じステージを除外して2択に
        nextScene = SelectRandomStage_Exclude(m_RandomEngine, m_RelationData.oldScene);
    }

    m_RelationData.nextScene = nextScene;
}
