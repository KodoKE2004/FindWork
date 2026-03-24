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
        // 総拍数に応じてボンバーUIの見た目サイズを補正する
        // ONE_MEASURE * 5 は「5小節ぶん」を最大基準として渡している
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
    
    m_RelationData.transTexture = nullptr;
    m_RelationData.ClearTransitionTexture();

    // 経過時間計測用のタイマー登録
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

    // 小節の切り替わり検知用に、前回値を退避
    m_PreciousMeasure = m_CurrentMeasure;

    RhythmBeat& rhythmBeat = Game::GetInstance().GetRhythmBeat();

    // 更新前の「四分音符単位でどこまで進んでいたか」を保持
    // GetBeatElapsed()が八分や独自単位基準なら / 2で四分音符換算している
    m_QuarterAdvance = rhythmBeat.GetBeatElapsed() / 2;

    // リズム本体を進める
    rhythmBeat.Update(tick);

    // 更新後の進行位置を四分音符基準で取得
    int quarterPreviewBeat = rhythmBeat.GetBeatElapsed() / 2;

    // 現在の経過拍数から小節番号を算出
    // m_BeatUnit を 一小節あたりの基準単位として使っている前提
    m_CurrentMeasure = rhythmBeat.GetBeatElapsed() / rhythmBeat.GetBeatConst().m_BeatUnit;

    // 残りの拍数を四分音符基準に揃える
    int rest = rhythmBeat.GetBeatRest() / 2;

    // 早回し処理
    if (m_isFastChange)
    {
        if (m_FastChangeState == FastChangeState::Filling)
        {
            m_FastChangeElapsed += tick;

            // 0.25拍分の短時間でゲージを一気に減らす演出
            const float beat = 0.25f;

            // 早回し開始から終了までの進行率[0, 1]
            const float progress = (beat > 0.0f)
                ? std::clamp(m_FastChangeElapsed / beat, 0.0f, 1.0f)
                : 1.0f;

            // 開始時のゲージ量から0へ向かって線形に減少
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
    // 四分音符が進んだ回数だけ区間更新する
    //-------------------------------
    for (int i = m_QuarterAdvance; i < quarterPreviewBeat; ++i)
    {
        // 新しい拍区間に入ったので、その区間内経過時間をリセット
        m_BomberElapsed = 0.0f;

        // 現在の累積拍位置
        const int currentIndex = rhythmBeat.GetBeatElapsed();

        // ひとつ前の補間結果を今回の始点にする
        m_SegmentFrom = m_FillRatio;

        const int baseBeats = rhythmBeat.GetBeatTotal();

        // 全体拍数に対して「次の拍まで進んだときの到達割合」を出す
        // +1.0f は「現在拍」ではなく「次の目標地点」まで含めるため
        float targetProgressNormal =
            std::clamp((static_cast<float>(currentIndex) + 1.0f) /
                static_cast<float>(baseBeats),
                0.0f, 1.0f);

        float targetProgress = targetProgressNormal;
        bool  useSpecial = false;

        // BASE_BEATS(24拍) を基準に、拍数が異なるステージでも
        // 最後の減少量の見た目バランスを揃えるための補正倍率
        float scaleMass = 0.95f;
        if (baseBeats != BASE_BEATS) {
            scaleMass = static_cast<float>(BASE_BEATS) / static_cast<float>(baseBeats);
        }

        // 特殊処理：
        // 残り3,2,1拍では通常の線形進行ではなく、
        // 指定値に合わせてゲージ終端の見た目を調整する
        switch (rest)
        {
        case 3: targetProgress = 1.0f - (0.075f * scaleMass); useSpecial = true; break;
        case 2: targetProgress = 1.0f - (0.030f * scaleMass); useSpecial = true; break;
        case 1: targetProgress = 1.0f;                        useSpecial = true; break;
        }

        // 残拍が 0 以下ならこのステージは終了扱い
        if (rest <= 0) {
            m_isChange = true;
        }

        // 今回区間の補間終点を確定
        m_SegmentTo = std::clamp(targetProgress, 0.0f, 1.0f);

        if (rest < 4)
        {
            // 残り3,2,1,0 のときだけカウントダウン演出を出す
            m_Bomber->SetCount(rest);
            m_Bomber->CountDown();

            if (rest == 0)
            {
                TextureManager* textureMgr = Game::GetInstance();

                PlaySE("explosion", std::nullopt);
                m_Bomber->SetTexture(textureMgr->GetTexture("Bomber/Explosion.png"));
            }
            else if (rest < 4 && rest > 0)
            {
                PlaySE("clock", std::nullopt);
            }
        }
    }

    // ボンバーの更新
    const float oneBeat = rhythmBeat.GetOneBeat();
    if (m_Bomber && oneBeat > 0.0f)
    {
        // 1拍の中でどこまで進んだかを 0～1 に正規化
        const float t = std::clamp(m_BomberElapsed / oneBeat, 0.0f, 1.0f);

        // 補間をなめらかにするための EaseOutQuart
        // 開始直後は速く、終わり際はゆっくり変化する
        const float e = Calculator::Easing::EaseOutQuart(t);

        // 今区間の開始値→終了値をイージング付きで補間
        m_FillRatio = m_SegmentFrom + (m_SegmentTo - m_SegmentFrom) * e;

        if (t >= 1.0f)
        {
            if (rest < 0)
            {
                m_isChange = true;
            }
        }

        // 内部では「進行率」を持ち、
        // 描画側には「残量」として渡したいので 1.0f から反転
        m_Bomber->SetFillRatio(1.0f - m_FillRatio);
    }
}

void GameSceneExe::FastChange()
{
    if (m_isFastChange) {
        return;
    }
    m_isFastChange = true;

    m_FastChangeState = FastChangeState::Filling;
    m_FastChangeElapsed = 0.0f;

    if (m_Bomber)
    {
        // 現在の残ゲージ量から早回し減少を始める
        m_FastChangeFill = m_Bomber->GetFillRatio();
    }
    else
    {
        m_FastChangeFill = 0.0f;
    }
    m_FastChangeStartFill = m_FastChangeFill;
}

void GameSceneExe::ChangeScene()
{
    Game::SetIsTickCount(true);
    auto& rhythmBeat = Game::GetRhythmBeat();

    // 次シーン移行まで 1拍ぶん待つための時間をセット
    GameToWait.duration = rhythmBeat.GetOneBeat();

    // その待ち時間ぶん Tick を進めて整合を取る
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

void GameSceneExe::RegisterReactionSE(std::string seName)
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


