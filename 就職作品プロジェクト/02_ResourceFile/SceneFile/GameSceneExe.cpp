#include "GameSceneExe.h"
#include "SceneList.h"
#include "input.h"
#include "DebugUI.h"
#include "Game.h"
#include "Model.h"
#include "Skydome.h"
#include "Collider.h"
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

    RhythmBeatConst beatConfig{};
    beatConfig.Setup(120.0f, 4, 1); // 120 BPM, 4/4 拍子
    m_RelationData.rhythmBeat.Initialize(beatConfig);

    m_BeatTimer.Initialize(12);

    // 値の反映
    m_TimerList.clear();

    SetTimer(&m_Elapsed);


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

    // 拍が進んでいた場合Uの更新を行う
    if (m_BeatTimer.GetCurrentBeat() > 1)
    {
        // 塗りつぶし率計算 0.13f分に4拍利用
        // 残りの0.87f分を8拍で均等に割る
        const float FILL_RATIO = (1.0f - 0.13f) * 0.125f;
        // 1拍の長さ（Tick数）の７割の時間で塗りつぶし率を進める
        const float duration   = m_RelationData.rhythmBeat.GetBeatConst().secondsPerBeat * 0.7f;
        const float t = std::clamp(m_Elapsed / max(duration, 0.0001f), 0.0f, 1.0f);
        const float ease = Math::Easing::EaseOutQuart(t);


    }



    // 進んだTick(拍数)を取得
    int advancedTick = m_RelationData.rhythmBeat.Update(tick);
    if (advancedTick > 0)
    {
        const int currentIndex = m_RelationData.rhythmBeat.GetBeatIndex();
        m_BeatTimer.Advance(currentIndex);
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

