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
   
    // 爆弾のリズム処理
    int advanceTick = m_RelationData.rhythmBeat.Update(tick);
    if (advanceTick > 0)
    {
        const int currentBeat = m_RelationData.rhythmBeat.GetBeatIndex();
        if (currentBeat > m_PreviousBeatIndex)
        {
            m_ElapsedBeats     += currentBeat - m_PreviousBeatIndex;
            m_PreviousBeatIndex = currentBeat;
        }

        float ratio = m_TimeGauge->GetFillRatio();
        ratio -= m_TimeGaugeStep * static_cast<float>(advanceTick);

        ratio = std::clamp(ratio, 0.13f, 1.0f);
        if (ratio == 0.13f && m_TimeGauge->GetCount() > 0){
            m_TimeGauge->ReadyExpo();
            // SEの再生
            PlaySE("clock",std::nullopt);
        }

        if (m_TimeGauge->IsReadyExpo()) {
            m_Number->SetCount(m_TimeGauge->GetCount());
            m_Number->ChangeTexture();
            m_TimeGauge->CountDown();
        }
        else {
            m_TimeGaugeRatio = ratio;
            m_TimeGauge->SetFillRatio(m_TimeGaugeRatio);
        }
    }

    // シーン遷移の条件

    // 速めにクリアした場合そこから一小節おいて遷移

    // 時間切れの場合
    if (m_isFastChange)
    {
        const int beatsPerBar = m_RelationData.rhythmBeat.GetBeatConst().beatUnit;
        const bool isValidBarLength = beatsPerBar > 0;
        const bool isBarChanged = isValidBarLength && (m_ElapsedBeats > 0) && (m_ElapsedBeats % beatsPerBar == 0);

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

