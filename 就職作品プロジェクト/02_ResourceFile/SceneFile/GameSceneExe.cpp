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

    // 初期化
    m_Difficulty    = 0;
    m_GameSpeedMass = 1.0f;
    m_isChange     = false;
    m_isFastChange = false;
    m_hasRequestedSceneChange = false;
    
    //===============================
    //   ゲームスピード倍率設定o
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
    
    RhythmBeatConst beatConfig{};
    beatConfig.Setup(120.0f, 4, 1); // 120 BPM, 4/4 拍子
    m_RelationData.rhythmBeat.Initialize(beatConfig);

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

 

    if (m_TimeGauge->GetFillRatio() < 1.0f &&
        !m_isChange) 
    {
        m_isChange = true;
    }
    
    CountTimer(tick);
   
    // 爆弾のリズム処理
    int advanceTick = m_RelationData.rhythmBeat.Update(tick);
    if (advanceTick > 0)
    {
        
        float ratio = m_TimeGauge->GetFillRatio();
        ratio -= m_TimeGaugeStep * static_cast<float>(advanceTick);

        ratio = std::clamp(ratio, 0.13f, 1.0f);
        if (ratio == 0.13f && m_TimeGauge->GetCount() > 0){
            m_TimeGauge->ReadyExpo();
            // SEの再生
            if (AudioManager* audioMgr = Game::GetInstance())
            {
                if (auto it = m_AudioList.find("clock"); it != m_AudioList.end())
                {
                    auto params = it->second.params;
                    if (it->second.loop)
                    {
                        params.loop.loopCount = XAUDIO2_LOOP_INFINITE;
                    }
                    audioMgr->Play("clock", params);
                }
                else
                {
                    audioMgr->Play("clock");
                }
            }
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

