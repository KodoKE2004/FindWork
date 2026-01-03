#include "GameSceneExe.h"
#include "SceneList.h"
#include "input.h"
#include "DebugUI.h"
#include "Game.h"
#include "Model.h"
#include "Skydome.h"
#include "Math.h"
#include <algorithm>




void GameSceneExe::Initialize()
{
    auto& instance = Game::GetInstance();
    TextureManager* textureMgr = instance;

    //-------------------------------
    //          メンバ初期化
    //-------------------------------
    m_isChange     = false;
    m_isFastChange = false;

    // リズムの定義
    RhythmBeatConst beatConfig{};
    beatConfig.Setup(120.0f, 4, 1); // 120 BPM, 4/4 拍子
    m_RelationData.rhythmBeat.Initialize(beatConfig);
    m_BeatTimer.Initialize(m_BaseBeats);

    // 値の反映
    m_TimerList.clear();
    SetTimer(&m_Elapsed);

    //-------------------------------
    //      音源の取得・生成
    //-------------------------------
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
    const int rest = m_BeatTimer.GetRestBeats();

    // 進んだTick(拍数)を取得
    int advancedTick = m_RelationData.rhythmBeat.Update(tick);
    // 拍が進んでいたらBeatTimerを進める
    for (int i = 0; i < advancedTick; ++i)
    {
        m_Elapsed = 0.0f;
        const int currentIndex = m_RelationData.rhythmBeat.GetBeatIndex();
        m_BeatTimer.Advance(currentIndex);


        m_SegmentFrom = m_FillRatio;

        float targetProgressNormal = 
            std::clamp((static_cast<float>(currentIndex) + 1.0f) / static_cast<float>(m_BaseBeats),
            0.0f, 1.0f);

        float targetProgress = targetProgressNormal;
        bool  useSpecial     = false;

        float scaleMass = 1.0f;
        if (m_BaseBeats != BASE_BEATS) {
            scaleMass = static_cast<float>(BASE_BEATS) / static_cast<float>(m_BaseBeats) ;
        }

        // 特殊処理：最後の4拍は指定数値で減るようにする
        switch (rest)
        {
        case 3: targetProgress = 1.0f - (0.10f * scaleMass); useSpecial = true; break;
        case 2: targetProgress = 1.0f - (0.05f * scaleMass); useSpecial = true; break;
        case 1: targetProgress = 1.0f;                       useSpecial = true; break;
        case 0: m_isChange     = true;                                          break;
        }

        m_SegmentTo = std::clamp(targetProgress, 0.0f, 1.0f);

        m_SpecialRest   = rest;
        m_SpecialActive = useSpecial;
        m_SpecialFrom   = m_FillRatio;
        m_SpecialTo     = targetProgress;

        std::cout << "Beat Index : " << rest << std::endl;
        if (m_SpecialRest < 4)
        {
            m_Bomber->SetCount(m_SpecialRest);
            m_Bomber->CountDown();
            if (rest == 0) { PlaySE("explosion", std::nullopt); }
            else           { PlaySE("clock"    , std::nullopt); }
        }
    }

    // ボンバーの更新
    const RhythmBeatConst& beatConst = m_RelationData.rhythmBeat.GetBeatConst();
    if (m_Bomber && beatConst.secondsPerBeat > 0.0f)
    {
        const float t = std::clamp(m_Elapsed / beatConst.secondsPerBeat, 0.0f, 1.0f);
        const float e = Math::Easing::EaseOutQuart(t);

        m_FillRatio = m_SegmentFrom + (m_SegmentTo - m_SegmentFrom) * e;

        if(t >= 1.0f)
        {
            if (rest < 0)
            {
                m_isChange = true;
            }
        }
        
        m_Bomber->SetFillRatio(1.0f - m_FillRatio);
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
}

