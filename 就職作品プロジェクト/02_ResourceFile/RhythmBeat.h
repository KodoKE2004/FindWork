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
    float           m_TickCounter = 0.0f;       // 経過時間の蓄積
    int             m_TickIndex   = 0;          // 現在のTick数
    int             m_Advance     = 0;          // 現在の拍子インデックス

public:
    RhythmBeat() = default;
    // 初期化
    void Initialize(const RhythmBeatConst& config);
    // 更新
    int Update(float deltaTime);

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

    const RhythmBeatConst& GetBeatConst()const
    {
        return m_Beat;
    }

    int GetAdvance() const {
        return m_Advance;
    }
};

// あらかじめ設定したビート数分更新をしていく。
class BeatTimer
{
private:
    int m_GameBeats     = 1;			// ゲーム内でとるビート数
    int m_BeatRest      = 0;			// 現在のビート数 GameBeats - m_BeatPrevious の差分を引く
    int	m_BeatPrevious  = 0;			// ビート数更新用カウンター
public:
    BeatTimer() = default;

    /// BeatTimer初期化
    /// @param 何拍とるか
    void Initialize(int gameBeats)
    {
        m_GameBeats = gameBeats;
        m_BeatRest = m_GameBeats;
        m_BeatPrevious = 0;
    }

    // 更新
    /// @param 経過拍数
    void Advance(const int beatIndex)
    {
        m_BeatRest     -= beatIndex - m_BeatPrevious;
        m_BeatPrevious  = beatIndex;
    }

    // 残りのビート数を取得
    int GetRestBeats()
    {   
        return m_BeatRest <= 0 ? 0 : m_BeatRest; 
    }

    // 経過したビート数を取得
    int GetCurrentBeat()
    {
        return m_GameBeats - m_BeatRest ;
    }

    bool IsBeatZero()
    {
        return m_BeatRest <= 0;
    }
};
