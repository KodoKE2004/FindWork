#include "GameSceneExe.h"
#include "SceneList.h"
#include "input.h"
#include "DebugUI.h"
#include "Game.h"
#include "Model.h"
#include "Skydome.h"
#include "Calculator.h"
#include <algorithm>
#include "TransitionBase.h"

GameSceneExe::GameSceneExe(Camera& cam) : Scene(cam)
{
}

void GameSceneExe::Initialize()
{
    auto& instance   = Game::GetInstance();
    auto& rhythmBeat = Game::GetRhythmBeat();

    if (m_Bomber)
    {
        m_Bomber->AdjustScaleByBeatTotal(rhythmBeat.GetBeatTotal(), ONE_MEASURE * 5);
    }

    //-------------------------------
    //          メンバ初期化 
    //-------------------------------
    m_isChange     = false;
    m_isFastChange = false;
    m_WasPlayBGM   = false;
    m_FastChangeState = FastChangeState::Filling;
    m_FastChangeFill = 0.0f;
    m_FastChangeStartFill = 0.0f;
    m_FastChangeElapsed   = 0.0f;    

    m_GameBomU = m_Bomber->GetBomberU();
    
    m_RelationData.transTexture = nullptr;
    m_RelationData.ClearTransitionTexture();

    // 値の反映
    m_TimerList.clear();
    SetTimer(&m_BomberElapsed);


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

    instance.StopBgm();
}

void GameSceneExe::Update(float tick)
{
    CountTimer(tick);
    m_PreciousMeasure = m_CurrentMeasure;

    RhythmBeat& rhythmBeat = Game::GetInstance().GetRhythmBeat();

    m_QuarterAdvance = rhythmBeat.GetBeatElapsed() / 2;

    // 進んだTick(拍数)を更新
    rhythmBeat.Update(tick);

    // 四分音符でリズムの計算をする
    int quarterPreviewBeat = rhythmBeat.GetBeatElapsed() / 2;

    // 1小節更新の検知
    m_CurrentMeasure = rhythmBeat.GetBeatElapsed() / rhythmBeat.GetBeatConst().m_BeatUnit;

    //四分音符基準の残拍数を取得
    int rest = rhythmBeat.GetBeatRest() / 2;

    // 早回し処理
    if (m_isFastChange)
    {
        if (m_FastChangeState == FastChangeState::Filling)
        {
            m_FastChangeElapsed += tick;

            const float beat = 1 * 0.25f;
            const float progress = (beat > 0.0f)
                ? std::clamp(m_FastChangeElapsed / beat, 0.0f, 1.0f)
                : 1.0f;
            m_FastChangeFill = max(0.0f, m_FastChangeStartFill * (1.0f - progress));

            if (m_Bomber)
            {
                m_Bomber->SetFillRatio(m_FastChangeFill);
            }
            if (m_FastChangeFill <= 0.0f)
            {
                m_FastChangeState = FastChangeState::ReadyToExplode;
                m_Bomber->SetFillRatio(0.0f);
            }

        }

        if (m_FastChangeState == FastChangeState::ReadyToExplode)
        {
            Explode();
        }
        return;
    }

    //-------------------------------
    // 拍が進んでいたらBeatTimerを進める
    //-------------------------------
    for (int i = m_QuarterAdvance; i < quarterPreviewBeat; ++i)
    {
        m_BomberElapsed = 0.0f;
        
        // 経過拍数の更新
        const int currentIndex = rhythmBeat.GetBeatElapsed();

        m_SegmentFrom = m_FillRatio;
        const int baseBeats = rhythmBeat.GetBeatTotal();
        float targetProgressNormal = 
            std::clamp((static_cast<float>(currentIndex) + 1.0f) /
                        static_cast<float>(baseBeats),
                        0.0f, 1.0f);

        float targetProgress = targetProgressNormal;
        bool  useSpecial     = false;

        float scaleMass = 0.95f;
        if (baseBeats != BASE_BEATS) {
            scaleMass = static_cast<float>(BASE_BEATS) / static_cast<float>(baseBeats) ;
        }

        // 特殊処理：最後の4拍は指定数値で減るようにする
        switch (rest)
        {
        case 3: targetProgress = 1.0f - (0.045f * scaleMass); useSpecial = true; break;
        case 2: targetProgress = 1.0f - (0.025f * scaleMass); useSpecial = true; break;
        case 1: targetProgress = 1.0f;                        useSpecial = true; break;
        }
        if (rest <= 0) {
            m_isChange = true;
        }
        m_SegmentTo = std::clamp(targetProgress, 0.0f, 1.0f);

        if (rest < 4)
        {
            m_Bomber->SetCount(rest);
            m_Bomber->CountDown();
            if (rest == 0)    { 
                TextureManager* textureMgr = Game::GetInstance();

                PlaySE("explosion", std::nullopt); 
                m_Bomber->SetTexture(textureMgr->GetTexture("Bomber/Explosion.png"));
            }
            else if(rest < 4 &&
                    rest > 0) { PlaySE("clock"    , std::nullopt); }
        }
    }

    // ボンバーの更新
    const float oneBeat = rhythmBeat.GetOneBeat();
    if(m_Bomber && oneBeat > 0.0f)
    {
        const float t = std::clamp(m_BomberElapsed / oneBeat, 0.0f, 1.0f);
        const float e = Calculator::Easing::EaseOutQuart(t);

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

void GameSceneExe::ChangeScene()
{
    Game::SetIsTickCount(true);
    auto& rhythmBeat = Game::GetRhythmBeat();
    GameToWait.duration = rhythmBeat.GetOneBeat();
    rhythmBeat.TickCount(GameToWait.duration);
    ChangeScenePop(GameToWait);
}

bool GameSceneExe::IsChangeMeasure()
{
    return m_PreciousMeasure != m_CurrentMeasure;
}

void GameSceneExe::Explode()
{
    if (m_FastChangeState == FastChangeState::Exploded)
    {
        return;
    }

    auto& instance = Game::GetInstance();
    TextureManager* textureMgr = instance;
    if (m_Bomber && textureMgr)
    {
        m_Bomber->SetTexture(textureMgr->GetTexture("Bomber/Explosion.png"));
    }

    PlaySE("explosion", std::nullopt);
    m_isChange = true;
    m_FastChangeState = FastChangeState::Exploded;
}

void GameSceneExe::RegesterReactionSE(std::string seName)
{
    AudioManager* audioMgr = Game::GetInstance();
    m_ReactionActive = audioMgr->Create(m_AudioList.at(seName));
}

void GameSceneExe::Finalize()
{
    auto& instance = Game::GetInstance();

    m_ReactionActive.reset();

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


