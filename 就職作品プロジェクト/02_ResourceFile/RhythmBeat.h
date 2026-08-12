#pragma once

// リズム計算に必要な定数群。
// このプロジェクトでは「時間(秒)」を直接進めるのではなく、
// BPM基準の拍/小節/Tickへ変換して進行を管理する。
struct RhythmBeatConst
{
    // 譜面・拍構造の定義
    float m_Bpm = 120.0f;          // 四分音符基準の BPM
    int   m_BeatsPerBar = 8;       // 1小節あたりの拍数（拍子の分子）
    int   m_BeatUnit = 8;          // 1拍の音価（4=四分音符, 8=八分音符）
    int   m_TicksPerBeat = 16;     // 1拍を何 Tick に分割するか
    int   m_HitsPerBar = 8;        // 1小節中のヒット基準数（演出/判定用）

    // 上記定義から導出されるキャッシュ値（秒系）
    float secondsPerBeat = 0.0f;    // 1拍の秒数
    float secondsPerBar = 0.0f;     // 1小節の秒数
    float secondsPerTick = 0.0f;    // 1Tick の秒数
    float ticksPerSecond = 0.0f;    // 1秒あたりの Tick 数

    float secondsPerHit = 0.0f;     // 1ヒット間隔の秒数
    float hitsPerSecond = 0.0f;     // 1秒あたりのヒット数

    void Setup(float bpm = 120.0f,
        int beatsPerBar = 8,
        int beatUnit = 8,
        int ticksPerBeat = 16,
        int hitsPerBar = 8)
    {
        m_Bpm = bpm;
        m_BeatsPerBar = beatsPerBar;
        m_BeatUnit = beatUnit;
        m_TicksPerBeat = ticksPerBeat;
        m_HitsPerBar = (hitsPerBar <= 0) ? 1 : hitsPerBar; // 0除算防止

        // BPM は四分音符基準のため、まず四分音符の秒数を作る。
        const float secondsPerQuarter = 60.0f / m_Bpm;

        // beatUnit が 8 の場合、1拍は四分音符の半分になる。
        secondsPerBeat = secondsPerQuarter * (4.0f / static_cast<float>(m_BeatUnit));

        // 小節は「1拍 × 拍数」。
        secondsPerBar = secondsPerBeat * static_cast<float>(m_BeatsPerBar);

        // Tick は拍をさらに分割した内部進行単位。
        secondsPerTick = secondsPerBeat / static_cast<float>(m_TicksPerBeat);
        ticksPerSecond = 1.0f / secondsPerTick;

        // ヒット間隔は小節を等分して作る。
        secondsPerHit = secondsPerBar / static_cast<float>(m_HitsPerBar);
        hitsPerSecond = 1.0f / secondsPerHit;
    }
};

struct RhythmBeatResult
{
    // Update前の経過拍数。
    int previousBeat = 0;

    // Update後の経過拍数。
    int currentBeat = 0;

    // このフレームで進んだ拍数。
    int advancedBeatCount = 0;

    bool HasAdvanced() const
    {
        return advancedBeatCount > 0;
    }
};


class RhythmBeat
{
private:
    RhythmBeatConst m_Beat{};
    float           m_TickCounter = 0.0f;   // 小数Tickの繰り越し用。フレーム落ち時の取りこぼし防止に使う
    int             m_TickIndex = 0;        // 経過した総 Tick 数
    int             m_BeatElapsed = 0;      // 経過拍
    int             m_BeatTotal = 0;        // 目標拍（ステージ長）

public:
    RhythmBeat() = default;

    // 新しい譜面設定を反映する。isTimeReset=true なら内部時間もリセットする。
    void Initialize(const RhythmBeatConst& config, bool isTimeReset);
    void Initialize(const RhythmBeatConst& config, bool isTimeReset, int limit);

    // tick(秒)を受け取り、内部拍進行を更新して「進んだ拍数」を返す。
    RhythmBeatResult Update(float tick);

    void SetTickCounter(float count)
    {
        m_TickCounter = count;
    }

    // トランジション待機等で、実時間を明示的に先送りしたいときに使う。
    void TickCount(float tick) {
        m_TickCounter += tick;
    }

    void SetElapsedBeat(int beats)
    {
        m_BeatElapsed = beats;
    }

    void SetBpm(float bpm)
    {
        if (bpm <= 0.0f) {
            return;
        }

        // BPM変更後は秒換算キャッシュを再計算する。
        m_Beat.Setup(
            bpm,
            m_Beat.m_BeatsPerBar,
            m_Beat.m_BeatUnit,
            m_Beat.m_TicksPerBeat,
            m_Beat.m_HitsPerBar);
    }

    float GetBpm() const
    {
        return m_Beat.m_Bpm;
    }

    int GetTotalTick() const
    {
        return m_TickIndex;
    }

    // 総 Tick を拍単位へ変換したインデックス。
    int GetBeatIndex() const
    {
        return m_TickIndex / m_Beat.m_TicksPerBeat;
    }

    // 現在拍の中での Tick 位置。
    int GetTickInBeat() const
    {
        return m_TickIndex % m_Beat.m_TicksPerBeat;
    }

    int GetBeatElapsed() const
    {
        return m_BeatElapsed;
    }

    int GetBeatTotal() const
    {
        return m_BeatTotal;
    }

    // ステージ全体の拍基準での残量。
    int GetBeatRest() const
    {
        return m_BeatTotal - m_BeatElapsed;
    }

    float GetOneBeat() const
    {
        return m_Beat.secondsPerBeat;
    }

    const RhythmBeatConst& GetBeatConst()const
    {
        return m_Beat;
    }

};
