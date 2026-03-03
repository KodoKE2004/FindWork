#include "GameSceneWait.h"
#include "Game.h"
#include "DebugUI.h"
#include "SceneList.h"
#include "TransitionBase.h"

#include <array>
#include <vector>
#include <random>
#include <cmath>
#include <algorithm>

GAME_PHASE GameSceneWait::m_CurrentGamePhase = GAME_PHASE::START;

//====================================
//            定数定義
//====================================

namespace
{
    // 二小節の拍数を基準にしているため、MeasureTwo = 32.0f 
    constexpr int MeasureOne = 16;
    constexpr int MeasureTwo = 32;
    float kGameUIStartPos  =   1024.0f;
    float kGameUIFinishPos = - 1024.0f;
    float kGameUICenterPos =      0.0f;

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
        { SCENE_NO::GAME_DODGE  , &PushGameStage<GameSceneDodge>  },
        { SCENE_NO::GAME_SHOT   , &PushGameStage<GameSceneShot>   },
        { SCENE_NO::GAME_TEXT   , &PushGameStage<GameSceneText>   },
        // { SCENE_NO::GAME_ROCKET , &PushGameStage<GameSceneRocket> },
        { SCENE_NO::GAME_GUNMAN , &PushGameStage<GameSceneGunman> },
    } };

    using StageList = std::vector<SCENE_NO>;

    StageList BuildStageCandidates_Exclude(SCENE_NO excludeScene)
    {
        StageList candidates;
        candidates.reserve(kStageEntries.size());

        for (const auto& entry : kStageEntries)
        {
            if (entry.scene != excludeScene) {
                candidates.emplace_back(entry.scene);
            }
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
        "Theme/Convey.png",
        // "Theme/Board.png",
        "Theme/Defeat.png",
    };

    const NVector3 kThemeScale[GAME_EXE_NUM] = {
        NVector3( 546.0f, 223.0f, 1.0f),
        NVector3( 557.0f, 217.0f, 1.0f),
        NVector3( 554.0f, 198.0f, 1.0f),
        // NVector3( 554.0f, 198.0f, 1.0f),
        NVector3( 554.0f, 198.0f, 1.0f),
    };

    constexpr float kStageTransitionDelay = 1.0f;

    constexpr uint32_t kLifeParticleCount = 12u;
    constexpr float    kLifeParticleSpeed = 400.0f;
    constexpr float    kLifeParticleLifeSec = 0.8f;

    constexpr float kLifeScaleUpAmount = 0.5f;
}

GameSceneWait::GameSceneWait(Camera& cam) : Scene(cam)
{
}

//====================================
//              初期化処理
//====================================

void GameSceneWait::Initialize()
{
    DebugUI::TEXT_CurrentScene = "GameSceneWait";

    auto& instance = Game::GetInstance();
    TextureManager* textureMgr = instance.GetInstance();

    m_CurrentGamePhase = GAME_PHASE::DO;

    // 最初の一度だけ or 指定したタイミングのみフラグを立てる
    m_IsFirstInitialized = !instance.HasFirstGameSceneWaitInitialized();
    instance.SetHasFirstGameSceneWaitInitialized(true);

    // 引き渡しデータのシーンの整理
    m_RelationData.ClearTransitionTexture();

    // スカイドーム初期化
    m_Skydome = AddObject<Skydome>(instance.GetCamera());
    m_Skydome->SetName("m_Skydome");
    m_Skydome->SetSkyDomeMode(true);
    m_Skydome->SetTexture(textureMgr->GetTexture("SkydomeSpace.png"));
    m_Skydome->SetRadius(500.0f);

    // 難易度アップ処理 
    ++m_RelationData.stageCount;

    m_TimerList.clear();
    SetTimer(&m_Tick);
    SetTimer(&m_DecrementLife.timer);
    m_WasPlayBGM         = false;
    m_QuarterAdvance     = 0;

    // ライフの数だけハートの生成
    const float lifePosX = - 200.0f;
    const float lifePosY = - 100.0f;

    m_LifeBaseScale = NVector3(100.0f, 100.0f, 1.0f);
    m_ShouldTransitionToStage = false;
    m_wasDecrementLife        = false;
    m_isPendingBpmChange      = false;

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

    m_isLifeScaleUp = false;
    for (auto life : m_LifeGame)
    {
        if (life)
        {
            life->SetRotate(life->GetRotate().x, life->GetRotate().y, 30.0f);
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

    RegisterAudio();

    //--------------------------------------------------------------------
    //                          リズムの初期化
    //--------------------------------------------------------------------
    RhythmBeatConst beatConfig{};
    beatConfig.Setup(Game::GetBgmBpm());
    m_UIGame.isBoot = false;

    // ゲームのリズムの初期化
    int gameBeats = MeasureTwo;
    if (m_RelationData.isClear) {
        Debug::Log("[[定期]]=====  ステージ成功  =====");
    }
    else {
        Debug::Log("[[定期]]=====  ステージ失敗  =====");
        if (m_RelationData.gameLife <= 1)
        {
            Debug::Log("=====  ゲームオーバー  =====");
            gameBeats += MeasureOne;
            m_CurrentGamePhase = GAME_PHASE::FINISH;
            m_UIGame.isBoot = true;
            RegisterGameUI(textureMgr->GetTexture("GameGuidance.png"), 2.0f, 2.0f);
        }
    }

    if (m_IsFirstInitialized)
    {
        Debug::Log("[[定期]]=====  ゲーム開始  =====");
        gameBeats += MeasureTwo / 2; 
        m_CurrentGamePhase = GAME_PHASE::START;
        m_UIGame.isBoot = true;
        RegisterGameUI(textureMgr->GetTexture("GameGuidance.png"), 1.0f, 1.0f);
    }


    //-------------------------------------------------------------------   
    //   難易度アップのタイミングでBPMを下げる or スピードアップのタイミングでBPMを上げる
    //-------------------------------------------------------------------   
    const int difficultyStageInterval = Game::GetDifficultyStageInterval();
    const int speedUpStageInterval = Game::GetSpeedUpStageInterval();

    // レベル変化時はBeatsを増やす
    // 難易度アップ時はBPMを下げる
    if (m_RelationData.stageCount % difficultyStageInterval == 0 && m_CurrentGamePhase != GAME_PHASE::FINISH)
    {

        m_isPendingBpmChange = true;
        // 難易度アップのときは長めに待つ
        gameBeats += MeasureTwo;
        m_CurrentGamePhase = GAME_PHASE::DO_UP_DIFFICULTY;
        m_UIGame.isBoot = true;
        RegisterGameUI(textureMgr->GetTexture("GameGuidance.png"), 1.0f, 2.0f);
        Debug::Log("[[定期]] 難易度アップ");

    }
    // スピードアップ時はBPMを上げる
    else if (m_RelationData.stageCount % speedUpStageInterval == 0 && m_CurrentGamePhase != GAME_PHASE::FINISH) {

        Debug::Log("[[定期]] スピードアップ");
        m_isPendingBpmChange = true;
        gameBeats += MeasureTwo;
        m_CurrentGamePhase = GAME_PHASE::DO_UP_SPEED;
        m_UIGame.isBoot = true;
        RegisterGameUI(textureMgr->GetTexture("GameGuidance.png"), 2.0f, 1.0f);

    }


    auto& rhythmBeat = Game::GetRhythmBeat();
    rhythmBeat.Initialize(beatConfig, false, gameBeats);

    Game::SetBgmBpm(beatConfig.m_Bpm);

    Debug::Log("===== クリアステージ数 : " + std::to_string(m_RelationData.stageCount) + " =====");

}

//====================================
//              更新処理
//====================================

void GameSceneWait::Update(float tick)
{   
    auto& rhythmBeat = Game::GetRhythmBeat();
    // リズムを取る    
    m_QuarterAdvance = rhythmBeat.GetBeatElapsed() / 2;
    rhythmBeat.Update(tick);
    int elapsedBeat      = rhythmBeat.GetBeatElapsed();
    int elapsedFourBeat  = elapsedBeat / 2;
    int restBeat     = rhythmBeat.GetBeatRest();

    GameUIMovement(elapsedBeat);

    // ライフをリズムに合わせて回転させる
    for (int i = m_QuarterAdvance; i < elapsedFourBeat; ++i)
    {
        // 1拍目のタイミングでBGM再生
        if (!m_WasPlayBGM)
        {
            Game::PlayBgm();
            m_WasPlayBGM = true;
        }
        
        if (restBeat <= ONE_MEASURE)
        {
            m_UIStage.isBoot       = true;
            // 四拍の長さ
            m_UIStage.movementTime = rhythmBeat.GetOneBeat() * 2.0f;
        }

        // 残り一拍のタイミングでステージ遷移フラグを立てる
        if (restBeat <=  5 && m_RelationData.gameLife > 0)
        {
            m_Theme->SetActive(true);
        }

        const bool shouldScaleLife =
            ((i + 1) % 2 == 1) &&
            !m_isLifeScaleUp &&
            !m_isLifeScaleDown;

        if (shouldScaleLife)
        {
            m_isLifeScaleUp = true;
            m_ScalingLifeDuration = rhythmBeat.GetBeatConst().secondsPerBeat * 0.25f;
        }
        
    }

    if (m_UIGame.isBoot)
    {
        
    }

    // ライフのスケーリング演出
    if (m_isLifeScaleUp || m_isLifeScaleDown)
    {
        LifeScaling();
    }

    if (rhythmBeat.GetBeatElapsed() >= rhythmBeat.GetBeatTotal() - 3)
    {
        m_ShouldTransitionToStage = true;
    }

    // ライフ減少処理
    if ( m_DecrementLife.IsTimeUp() &&
        !m_wasDecrementLife         &&
        !m_RelationData.isClear)
    {
        // ライフを減らす
        m_RelationData.gameLife -= 1u;
        LifeDecrement();
        m_wasDecrementLife = true;
    }

    // タイマー更新処理
    CountTimer(tick);

    // デバッグ用　終わったら消す予定のreturn
    if (m_RelationData.gameLife == 0u && m_ShouldTransitionToStage)
    {
        // ライフが0になったらリザルトシーンへ
        Game::SetIsTickCount(false);
        ChangeScenePush<ResultScene>(WaitToResult);
    }
    if (m_ShouldTransitionToStage)
    {
        // StartNextStageTransition();
    }
}

//====================================
//              描画処理
//====================================

void GameSceneWait::Draw()
{
    Scene::Draw();
}

//====================================
//              解放処理
//====================================

void GameSceneWait::Finalize()
{
    auto& instance = Game::GetInstance();

    // このシーンのオブジェクトを削除する
    for (auto o : m_MySceneObjects) {
        DeleteObject(o);
    }
    m_MySceneObjects.clear();
    // オーディオの停止と登録のクリア
    if (AudioManager* audioManager = instance)
    {
        for (const auto& [key, config] : m_AudioList)
        {
            audioManager->StopAllByName(key);
        }
        m_AudioList.clear();
    }
}

//====================================
//         GameUI登録
//====================================

void GameSceneWait::RegisterGameUI(pShared<Texture> texture, float u, float v)
{
    auto& instance = Game::GetInstance();
    m_GameUI = AddObject<Square>(instance.GetCamera());
    m_GameUI->SetTexture(texture);
    m_GameUI->SetName("m_GameUI");
    m_GameUI->SetScale(768.0f, 512.0f, 1.0f);
    m_GameUI->SetPos(kGameUIStartPos, 50.0f, 0.0f);
    m_GameUI->SetUV(u, v, 2.0f, 2.0f);
    m_UIGame.phase = UI_PHASE::NONE;

    auto& rhythmBeat = Game::GetRhythmBeat();

    // 移動にかかる時間をビート数から計算
    m_UIGame.movementTime = rhythmBeat.GetBeatConst().secondsPerBeat * 6;
    
}

//====================================
//          　GameUI移動演出
//====================================

void GameSceneWait::GameUIMovement(int elapsedBeat)
{
    if (!m_UIGame.isBoot || !m_GameUI) {
        return;
    }

    // ゲームのフェーズで順序を替える
    int phaseBeats = 0;
    if (m_CurrentGamePhase > GAME_PHASE::START) {
        phaseBeats += 16; // ライフの管理は最初の一小節で完結させるためその分を加算
    }

    int kStartWaitBeat      = 2  + phaseBeats;
    int kSlideInEndBeat     = 8  + phaseBeats;    //  2 wait + 6 move
    int kCenterWaitEndBeat  = 24 + phaseBeats;   // +16 wait
    int kSlideOutEndBeat    = 30 + phaseBeats;   // + 6 move
    int kSequenceEndBeat    = 32 + phaseBeats;   // + 2 wait

    if (elapsedBeat < kStartWaitBeat)
    {
        m_UIGame.phase = UI_PHASE::NONE;
        m_GameUI->SetPos(kGameUIStartPos, 50.0f, 0.0f);
        return;
    }

    if (elapsedBeat >= kSequenceEndBeat)
    {
        m_UIGame.phase = UI_PHASE::NONE;
        m_GameUI->SetPos(kGameUIFinishPos, 50.0f, 0.0f);
        m_UIGame.isBoot = false;
        return;
    }

    if (elapsedBeat < kSlideInEndBeat)
    {
        if (m_isPendingBpmChange) {
            RhythmBeatConst beatConfig{};
            const float speedUpBpmIncrease = Game::GetSpeedUpBpmIncrease();
            if (m_CurrentGamePhase == GAME_PHASE::DO_UP_SPEED) {
                beatConfig.Setup(Game::GetBgmBpm() + speedUpBpmIncrease);
            }
            else if (m_CurrentGamePhase == GAME_PHASE::DO_UP_DIFFICULTY) {
                beatConfig.Setup(Game::GetBgmBpm() - speedUpBpmIncrease);
            }

            auto& rhythmBeat = Game::GetRhythmBeat();
            rhythmBeat.Initialize(beatConfig, false);
            rhythmBeat.SetElapsedBeat(MeasureOne);
            Game::SetBgmBpm(beatConfig.m_Bpm);
            m_isPendingBpmChange = false;
            m_UIGame.movementTime = rhythmBeat.GetBeatConst().secondsPerBeat * 6;
        }
        if (m_UIGame.phase != UI_PHASE::SLIDE_IN)
        {
            m_UIGame.phase = UI_PHASE::SLIDE_IN;
            m_UIGame.moveValueX = kGameUICenterPos - kGameUIStartPos;
            m_UIGame.movementElapsed = 0.0f;
            m_GameUI->SetPos(kGameUIStartPos, 50.0f, 0.0f);
        }
    }
    else if (elapsedBeat < kCenterWaitEndBeat)
    {
        m_UIGame.phase = UI_PHASE::WAIT;
        m_GameUI->SetPos(kGameUICenterPos, 50.0f, 0.0f);
        return;
    }
    else if (elapsedBeat < kSlideOutEndBeat)
    {
        if (m_UIGame.phase != UI_PHASE::SLIDE_OUT)
        {
            m_UIGame.phase = UI_PHASE::SLIDE_OUT;
            m_UIGame.moveValueX = kGameUIFinishPos - kGameUICenterPos;
            m_UIGame.movementElapsed = 0.0f;
            m_GameUI->SetPos(kGameUICenterPos, 50.0f, 0.0f);
        }
    }
    else
    {
        m_UIGame.phase = UI_PHASE::NONE;
        m_GameUI->SetPos(kGameUIFinishPos, 50.0f, 0.0f);
        return;
    }

    m_UIGame.movementElapsed += Application::GetDeltaTime();
    float t = m_UIGame.movementElapsed / m_UIGame.movementTime;
    t = std::min(t, 1.0f);

    float baseX = (m_UIGame.phase == UI_PHASE::SLIDE_IN) ? kGameUIStartPos : kGameUICenterPos;
    float elapsedX = baseX + (m_UIGame.moveValueX * Calculator::Easing::EaseInQuad(t));
    m_GameUI->SetPos(elapsedX, 50.0f, 0.0f);
}

//====================================
//      次のステージ選択とシーン遷移処理
//====================================

void GameSceneWait::StartNextStageTransition()
{
    Game::SetIsTickCount(true);
    RhythmBeat& rhythmBeat = Game::GetRhythmBeat();
    WaitToGame.duration = rhythmBeat.GetOneBeat();
    rhythmBeat.TickCount(WaitToGame.duration);

    // シーン遷移処理
    switch (m_RelationData.nextScene)
    {
    case SCENE_NO::GAME_DODGE : ChangeScenePush<GameSceneDodge>  (WaitToGame); break;
    case SCENE_NO::GAME_SHOT  : ChangeScenePush<GameSceneShot>   (WaitToGame); break;
    case SCENE_NO::GAME_TEXT  : ChangeScenePush<GameSceneText>   (WaitToGame); break;
    // case SCENE_NO::GAME_ROCKET: ChangeScenePush<GameSceneRocket>(WaitToGame); break;
    case SCENE_NO::GAME_GUNMAN: ChangeScenePush<GameSceneGunman>(WaitToGame); break;
    default: return;
    }
}

//====================================
//              ライフ減少処理
//====================================

void GameSceneWait::LifeDecrement()
{
    if (!m_LifeGame.empty())
    DeleteObject(m_LifeGame.back());
    m_LifeGame.pop_back();
    --m_LifeCount;
}

//====================================
//         ライフスケーリング演出
//====================================

void GameSceneWait::LifeScaling()
{
    namespace Calculator = Calculator::Easing;

    m_ScalingLifeElapsed += Application::GetDeltaTime();

    const NVector3 lifeScaleUp = m_LifeBaseScale + m_LifeBaseScale * kLifeScaleUpAmount;
    NVector3 targetScale = m_LifeBaseScale;

    // 大きくなるフェーズ
    if (m_isLifeScaleUp)
    {
        targetScale = Calculator::ScalingObject(
            m_ScalingLifeElapsed,
            m_ScalingLifeDuration,
            m_LifeBaseScale,
            lifeScaleUp,
            EASING_TYPE::IN_QUAD);

        if (m_ScalingLifeElapsed >= m_ScalingLifeDuration)
        {
            m_isLifeScaleUp = false;
            m_isLifeScaleDown = true;
            targetScale = lifeScaleUp;
            m_ScalingLifeElapsed = 0.0f;
        }
    }
    // 小さくなるフェーズ
    else if (m_isLifeScaleDown)
    {
        targetScale = Calculator::ScalingObject(
            m_ScalingLifeElapsed,
            m_ScalingLifeDuration,
            lifeScaleUp,
            m_LifeBaseScale,
            EASING_TYPE::IN_QUAD);

        if (m_ScalingLifeElapsed >= m_ScalingLifeDuration)
        {
            m_isLifeScaleUp = false;
            m_isLifeScaleDown = false;
            targetScale = m_LifeBaseScale;
            m_ScalingLifeElapsed = 0.0f;
        }
    }

    for (auto life : m_LifeGame)
    {
        if (life)
        {
            life->SetScale(targetScale);
        }
    }
}

//====================================
//         　　StageUI移動変更
//====================================

void GameSceneWait::StageCountUIMovement(float tick)
{
    auto rhythmBeat     = Game::GetRhythmBeat();
    const float OneBeat = rhythmBeat.GetOneBeat() * 2.0f;
    m_UIStage.movementElapsed += tick;

    bool isElapsedOneBeat = m_UIStage.movementElapsed >= OneBeat;
    
    
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
