#pragma once

// リズムの変数
// 
struct RhythmBeatConst
{
    // 入力値
    float m_Bpm = 120.0f;        // BPM (Beats Per Minute)
    int   m_BeatUnit = 4;        // 拍子の分母 (4 = 4分音符, 8 = 8分音符, etc.)
    int   m_TicksPerBeat = 16;   // 1拍を何分割するか

    // 計算結果
    float secondsPerBeat = 0.0f; // 1拍の長さ（秒）
    float secondsPerBar  = 0.0f; // 1小節の長さ（秒）
    float secondsPerTick = 0.0f; // 1Tickの長さ（秒）
    float ticksPerSecond = 0.0f; // 1秒あたりに進むTick数

    // Setup（セットアップ）:
    //     設定をまとめて反映する関数名によく使われる
    void Setup(float bpm, int beatsPerBar = 4, int ticksPerBeat = 16)
    {
        m_Bpm = bpm;
        m_BeatUnit = beatsPerBar;
        m_TicksPerBeat = ticksPerBeat;

        // 1拍の長さ（秒） = 60 / BPM
        secondsPerBeat = 60.0f / m_Bpm;

        // 1小節の長さ（秒） = 1拍 * 拍数
        secondsPerBar = secondsPerBeat * static_cast<float>(m_BeatUnit);

        // 1Tickの長さ（秒） = 1拍 / Tick数
        secondsPerTick = secondsPerBeat / static_cast<float>(m_TicksPerBeat);

        // 1秒あたりのTick数 = 1 / secondsPerTick
        ticksPerSecond = 1.0f / secondsPerTick;
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
    void Initialize(const RhythmBeatConst& config);
    void Initialize(const RhythmBeatConst& config,int limit);
    // 経過拍数の取得を行う
    int Update(float tick);

    void SetTickCounter(float count)
    {
        m_TickCounter = count;
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
