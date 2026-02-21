#pragma once

// リズムの変数  
struct RhythmBeatConst
{
    // 入力
    float m_Bpm = 120.0f;          // 四分音符基準のBPM
    int   m_BeatsPerBar = 4;       // 分子：小節の拍数
    int   m_BeatUnit = 4;          // 分母：4=四分, 8=八分...
    int   m_TicksPerBeat = 16;     // 1拍を何分割するか
    int   m_HitsPerBar = 4;        // ★1小節に何回叩く(イベント)か（変則OK）

    // 計算結果
    float secondsPerBeat = 0.0f;   // 1拍（分母の音符）の秒
    float secondsPerBar  = 0.0f;   // 1小節の秒
    float secondsPerTick = 0.0f;   // 1Tickの秒
    float ticksPerSecond = 0.0f;   // 1秒あたりTick

    float secondsPerHit = 0.0f;   // ★1回叩く間隔（小節を hitsPerBar で割る）
    float hitsPerSecond = 0.0f;   // ★1秒あたり叩く回数

    void Setup(float bpm = 120.0f,
        int beatsPerBar  = 8,
        int beatUnit     = 8,
        int ticksPerBeat = 16,
        int hitsPerBar   = 8)
    {
        m_Bpm = bpm;
        m_BeatsPerBar   = beatsPerBar;
        m_BeatUnit      = beatUnit;
        m_TicksPerBeat  = ticksPerBeat;
        m_HitsPerBar    = (hitsPerBar <= 0) ? 1 : hitsPerBar; // 0除算防止

        // 四分音符の秒
        const float secondsPerQuarter = 60.0f / m_Bpm;

        // 1拍（分母音符）の秒：4→8なら半分
        secondsPerBeat = secondsPerQuarter * (4.0f / static_cast<float>(m_BeatUnit));

        // 小節秒：拍数（分子）×1拍秒
        secondsPerBar = secondsPerBeat * static_cast<float>(m_BeatsPerBar);

        // Tick秒
        secondsPerTick = secondsPerBeat / static_cast<float>(m_TicksPerBeat);
        ticksPerSecond = 1.0f / secondsPerTick;

        // ★叩く間隔（小節を hits で割る）
        secondsPerHit = secondsPerBar / static_cast<float>(m_HitsPerBar);
        hitsPerSecond = 1.0f / secondsPerHit;
    }
};
class RhythmBeat
{
private:
    RhythmBeatConst m_Beat{};
    float           m_TickCounter = 0.0f;   // 経過時間の蓄積
    int             m_TickIndex   = 0;      // 現在のTick数
    int             m_Advance     = 0;      // 現在の拍子インデックス
    int             m_BeatElapsed = 0;      // 経過拍数
    int             m_BeatTotal   = 0;      // 総Tick数

public:
    RhythmBeat() = default;
    // 初期化
    void Initialize(const RhythmBeatConst& config, bool isTimeReset);
    void Initialize(const RhythmBeatConst& config, bool isTimeReset, int limit);
    // 経過拍数の取得を行う
    int Update(float tick);

    void SetTickCounter(float count)
    {
        m_TickCounter = count;
    }

    void TickCount(float tick) {
        m_TickCounter += tick;
    }

    void SetElapsedBeat(int beats)
    {
        m_BeatElapsed = beats;
    }

    void SetBpm(float bpm)
    {
        m_Beat.m_Bpm = bpm;
        // BPM変更に伴い、関連する時間計算を更新
        m_Beat.Setup(m_Beat.m_Bpm, m_Beat.m_BeatUnit, m_Beat.m_TicksPerBeat);
    }

    float GetBpm() const
    {
        return m_Beat.m_Bpm;
    }

    // 現在のTickを取得
    int GetTotalTick() const
    {
        return m_TickIndex;
    }

    // 現在の拍子インデックスを取得
    // 今が何泊目かを返す
    int GetBeatIndex() const
    {
        return m_TickIndex / m_Beat.m_TicksPerBeat;
    }

    // 現在の拍子内のTick位置を取得
    // 今の拍子の中で何Tick目かを返す
    int GetTickInBeat() const
    {
        return m_TickIndex % m_Beat.m_TicksPerBeat;
    }

    // 経過拍数を取得
    int GetBeatElapsed() const
    {
        return m_BeatElapsed;
    }

    // 総拍数を取得
    int GetBeatTotal() const
    {
        return m_BeatTotal;
    }

    // 残拍数を取得
    int GetBeatRest() const
    {
        return m_BeatTotal - m_BeatElapsed;
    }

    // 一拍の長さを取得
    float GetOneBeat() const 
    {
        return m_Beat.secondsPerBeat;
    }

    const RhythmBeatConst& GetBeatConst()const
    {
        return m_Beat;
    }

    int GetAdvance() const {
        return m_Advance;
    }
};
