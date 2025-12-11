#include "GameSceneExe.h"
#include "SceneList.h"
#include "input.h"
#include "DebugUI.h"
#include "Game.h"
#include "ObjectFile/Model.h"
#include "ObjectFile/Skydome.h"
#include <algorithm>

void GameSceneExe::Initialize()
{
    auto& instance = Game::GetInstance();
    TextureManager* textureMgr = instance;
    //-------------------------------
    //          メンバ初期化
    //-------------------------------
    m_Difficulty    = 0;
    m_GameSpeedMass = 1.0f;
    m_isChange     = false;
    m_isFastChange = false;
    m_hasRequestedSceneChange = false;
    m_ShouldSkipNextBoundary  = false;
    
    //===============================
    //   ゲームスピード倍率設定
    //   ステージクリア数 n
    //   n && 8 == 0 なら難易度アップ 
    //   n && 4 == 0 ならスピードアップ
    //===============================
   
    ++m_RelationData.stageCount;
    // 難易度 0 ~
    if (m_RelationData.stageCount % 8 == 0) {
        int difficulty = m_RelationData.stageCount / 8;
        m_Difficulty = difficulty;
    }
    // スピード
    else if(m_RelationData.stageCount % 4 == 0){
        float speedMass = ((float)m_RelationData.stageCount / 8.0f) + 1.0f;
        m_GameSpeedMass = 1.0f + (speedMass * 0.2f);
    }

    // 値の反映
    m_TimerList.clear();
    
    // スピードゲージ関連初期化
    RhythmBeatConst beatConfig{};
    beatConfig.Setup(120.0f, 4, 1); // 120 BPM, 4/4 拍子
    m_RelationData.rhythmBeat.Initialize(beatConfig);

    m_ElapsedBeats = 0;                                                   // 経過拍数
    m_PreviousBeatIndex = 0;                                              // 前回の拍数
    m_ForcedReturnBeatCount = beatConfig.beatUnit * ForcedReturnMeasures; // 強制リターンまでの拍数

    const auto& beatConst = m_RelationData.rhythmBeat.GetBeatConst();
    const float secondsPerBeat = beatConst.secondsPerBeat;

    m_OneMeasure = secondsPerBeat * static_cast<float>(beatConst.beatUnit);

    constexpr float kGaugeFillPortion = 0.7f;
    m_GaugeAnimDuration = secondsPerBeat * kGaugeFillPortion;

    m_GaugeBeatElapsed = 0.0f;
    m_GaugeStartRatio = 1.0f;
    m_GaugeTargetRatio = 1.0f;

    SetTimer(&m_GaugeBeatElapsed);

    m_TimeGaugeRatio = 1.0f;
    if (m_TimeGauge)
    {
        m_TimeGauge->SetFillRatio(1.0f);
    }

    m_TimeGaugeStep = 1.0f / static_cast<float>(GaugeTicks);

    m_TimerList.clear();

    PlayParams clockParam{};
    clockParam.volume = DEFAULT_VOLUME;
    m_AudioList.emplace("clock", AudioConfig(L"SE/Clock.wav", clockParam, false, false));
    
    PlayParams exploParam{};
    exploParam.volume = DEFAULT_VOLUME;
    m_AudioList.emplace("explosion", AudioConfig(L"SE/Explosion.wav", exploParam, false, false));

    if (AudioManager* audioMgr = instance)
    {
        for (const auto& [key, config] : m_AudioList)
        {
            audioMgr->Add(key, config.filePath);
        }
    }
}

void GameSceneExe::Update(float tick)
{
    CountTimer(tick);

    const float minRatio = 0.13f;

    if (m_TimeGauge && !m_TimeGauge->IsReadyExpo() && m_GaugeAnimDuration > 0.0f)
    {
        m_GaugeBeatElapsed += tick;

        float clampedTarget = std::clamp(m_GaugeTargetRatio, minRatio, 1.0f);
        float ratio = clampedTarget;

        if (m_GaugeBeatElapsed < m_GaugeAnimDuration)
        {
            float t = m_GaugeBeatElapsed / m_GaugeAnimDuration;

            auto EaseOutCubic = [](float x)
                {
                    float inv = 1.0f - x;
                    return 1.0f - inv * inv * inv;
                };

            float eased = EaseOutCubic(t);
            ratio = m_GaugeStartRatio + (clampedTarget - m_GaugeStartRatio) * eased;
        }
        else
        {
            ratio = clampedTarget;
        }

        m_TimeGaugeRatio = ratio;
        m_TimeGauge->SetFillRatio(m_TimeGaugeRatio);
    }

    int advanceTick = m_RelationData.rhythmBeat.Update(tick);
    if (advanceTick > 0)
    {
        const int currentBeat = m_RelationData.rhythmBeat.GetBeatIndex();
        if (currentBeat > m_PreviousBeatIndex)
        {
            m_ElapsedBeats += currentBeat - m_PreviousBeatIndex;
            m_PreviousBeatIndex = currentBeat;
        }

        m_GaugeBeatElapsed = 0.0f;
        m_GaugeStartRatio = m_TimeGaugeRatio;

        float nextTarget = m_GaugeStartRatio - m_TimeGaugeStep * static_cast<float>(advanceTick);
        m_GaugeTargetRatio = std::clamp(nextTarget, minRatio, 1.0f);

        if (m_GaugeTargetRatio <= minRatio &&
            m_TimeGauge->GetCount() > 0 &&
            !m_TimeGauge->IsReadyExpo())
        {
            m_TimeGauge->ReadyExpo();
            PlaySE("clock", std::nullopt);
        }
        else if (m_TimeGauge->GetCount() == 0)
        {
            PlaySE("explosion", std::nullopt);
        }

        if (m_TimeGauge->IsReadyExpo())
        {
            m_Number->SetCount(m_TimeGauge->GetCount());
            m_Number->ChangeTexture();
            m_TimeGauge->CountDown();
        }
    }

    if (m_isFastChange)
    {
        const int beatsPerBar = m_RelationData.rhythmBeat.GetBeatConst().beatUnit;
        const bool isValidBarLength = beatsPerBar > 0;
        const bool isBarChanged = isValidBarLength &&
            (m_ElapsedBeats > 0) &&
            (m_ElapsedBeats % beatsPerBar == 0);

        if (isBarChanged)
        {
            const int currentMeasureIndex = m_ElapsedBeats / beatsPerBar;
            if (currentMeasureIndex <= 2)
            {
                if (m_ShouldSkipNextBoundary)
                {
                    m_ShouldSkipNextBoundary = false;
                }
                else
                {
                    m_hasRequestedSceneChange = true;
                    m_isChange = true;
                }
            }
        }
    }

    if (m_ForcedReturnBeatCount > 0 && m_ElapsedBeats >= m_ForcedReturnBeatCount)
    {
        m_hasRequestedSceneChange = true;
    }

    if (m_hasRequestedSceneChange)
    {
        m_isChange = true;
    }
}

void GameSceneExe::Finalize()
{
    auto& instance = Game::GetInstance();
#ifdef _DEBUG
    instance.m_Grid.SetEnabled(false);
#endif

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

void GameSceneExe::StageChangeFast()
{
    m_isFastChange = true;
    const int beatsPerBar = m_RelationData.rhythmBeat.GetBeatConst().beatUnit;
    if (beatsPerBar > 0)
    {
        const int beatInBar = m_ElapsedBeats % beatsPerBar;
        m_ShouldSkipNextBoundary = (beatsPerBar - beatInBar == 1);
    }
    else
    {
        m_ShouldSkipNextBoundary = false;
    }
}

