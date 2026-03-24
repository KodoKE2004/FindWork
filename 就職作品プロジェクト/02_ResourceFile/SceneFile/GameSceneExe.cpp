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
    auto& instance = Game::GetInstance();
    auto& rhythmBeat = Game::GetRhythmBeat();

    if (m_Bomber)
    {
        // ステージ拍数に応じてボンバーUIの見た目サイズを補正する。
        // ONE_MEASURE * 5 は「5小節相当」を基準として渡している。
        m_Bomber->AdjustScaleByBeatTotal(rhythmBeat.GetBeatTotal(), ONE_MEASURE * 5);
    }

    //-------------------------------
    // 状態フラグ初期化
    //-------------------------------
    m_isChange = false;
    m_isFastChange = false;
    m_WasPlayBGM = false;
    m_FastChangeState = FastChangeState::Filling;
    m_FastChangeFill = 0.0f;
    m_FastChangeStartFill = 0.0f;
    m_FastChangeElapsed = 0.0f;

    m_RelationData.transTexture = nullptr;
    m_RelationData.ClearTransitionTexture();

    // 区間補間用の経過時間タイマーを登録。
    m_TimerList.clear();
    SetTimer(&m_BomberElapsed);

    //-------------------------------
    // SE登録
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

    // Exe中は場面ごとのテンポ感を明確にするため、開始時にBGMを一旦止める。
    instance.StopBgm();
}

void GameSceneExe::Update(float tick)
{
    CountTimer(tick);

    //-------------------------------
    // 小節切り替え判定用の更新前情報を退避
    //-------------------------------
    m_PreciousMeasure = m_CurrentMeasure;

    RhythmBeat& rhythmBeat = Game::GetInstance().GetRhythmBeat();

    // 更新前の四分音符基準進行を保持。
    // GetBeatElapsed() は内部拍基準なので /2 で四分音符基準に揃える。
    m_QuarterAdvance = rhythmBeat.GetBeatElapsed() / 2;

    // リズム本体の進行更新。
    rhythmBeat.Update(tick);

    // 更新後の進行位置（四分音符基準）。
    int quarterPreviewBeat = rhythmBeat.GetBeatElapsed() / 2;

    // 現在の経過拍から小節番号を算出。
    m_CurrentMeasure = rhythmBeat.GetBeatElapsed() / rhythmBeat.GetBeatConst().m_BeatUnit;

    // 残拍も四分音符基準に統一して扱う。
    int rest = rhythmBeat.GetBeatRest() / 2;

    //-------------------------------
    // 先行クリア中は通常進行をスキップし専用演出のみ実行
    //-------------------------------
    if (m_isFastChange)
    {
        if (m_FastChangeState == FastChangeState::Filling)
        {
            m_FastChangeElapsed += tick;

            // 先行クリア時は 0.25 秒で一気に減衰させ、テンポ感を崩さない。
            const float fastChangeDuration = 0.25f;

            // 早回し開始からの進行率 [0,1]。
            const float progress = (fastChangeDuration > 0.0f)
                ? std::clamp(m_FastChangeElapsed / fastChangeDuration, 0.0f, 1.0f)
                : 1.0f;

            // 開始時の残量から 0 へ線形減衰。
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
    // 四分音符が進んだ回数だけ区間更新
    // （フレーム落ち時の取りこぼし防止）
    //-------------------------------
    for (int i = m_QuarterAdvance; i < quarterPreviewBeat; ++i)
    {
        // 新しい区間に入ったので、区間内経過時間をリセット。
        m_BomberElapsed = 0.0f;

        // 現在の累積拍位置。
        const int currentIndex = rhythmBeat.GetBeatElapsed();

        // 直前区間の終了値を次区間の始点にする。
        m_SegmentFrom = m_FillRatio;

        const int baseBeats = rhythmBeat.GetBeatTotal();

        // 総拍に対して「次の目標拍まで進んだときの進行率」を求める。
        // +1.0f は現拍ではなく「次目標地点」まで含めるため。
        float targetProgressNormal =
            std::clamp((static_cast<float>(currentIndex) + 1.0f) /
                static_cast<float>(baseBeats),
                0.0f, 1.0f);

        float targetProgress = targetProgressNormal;
        bool  useSpecial = false;

        // BASE_BEATS(24拍)基準と異なるステージでは、
        // 終盤演出の見た目バランスを崩さないため補正係数を入れる。
        float scaleMass = 0.95f;
        if (baseBeats != BASE_BEATS) {
            scaleMass = static_cast<float>(BASE_BEATS) / static_cast<float>(baseBeats);
        }

        // 終盤（残り3/2/1拍）は単純線形ではなく、
        // 爆発演出へ緊張感を繋ぐ終端位置へ寄せる。
        switch (rest)
        {
        case 3: targetProgress = 1.0f - (0.075f * scaleMass); useSpecial = true; break;
        case 2: targetProgress = 1.0f - (0.030f * scaleMass); useSpecial = true; break;
        case 1: targetProgress = 1.0f;                        useSpecial = true; break;
        }

        if (rest <= 0) {
            m_isChange = true;
        }

        // 現区間の終端値を確定。
        m_SegmentTo = std::clamp(targetProgress, 0.0f, 1.0f);

        if (rest < 4)
        {
            // 残り 3,2,1,0 ではカウントダウン演出を出す。
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

    //-------------------------------
    // ボンバーUIの補間更新
    //-------------------------------
    const float oneBeat = rhythmBeat.GetOneBeat();
    if (m_Bomber && oneBeat > 0.0f)
    {
        // 1拍内でどこまで進んだかを 0〜1 に正規化。
        const float t = std::clamp(m_BomberElapsed / oneBeat, 0.0f, 1.0f);

        // 区間の頭を速く・末尾をゆっくりにして視認性を上げる。
        const float e = Calculator::Easing::EaseOutQuart(t);

        // 区間始点→終点をイージングで補間。
        m_FillRatio = m_SegmentFrom + (m_SegmentTo - m_SegmentFrom) * e;

        if (t >= 1.0f)
        {
            if (rest < 0)
            {
                m_isChange = true;
            }
        }

        // 内部は進行率、UIは残量表示なので反転して渡す。
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
        // 現在の残量から減衰演出を開始する。
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

    // 次シーン移行まで 1拍ぶん待ち、演出終端を見せ切る。
    GameToWait.duration = rhythmBeat.GetOneBeat();

    // 待機時間ぶんを Rhythm 側にも加算し、拍基準の整合を保つ。
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

    // このシーンで登録したSEを停止して解放。
    if (AudioManager* audioManager = instance)
    {
        for (const auto& [key, config] : m_AudioList)
        {
            audioManager->StopAllByName(key);
        }
        m_AudioList.clear();
    }
}


