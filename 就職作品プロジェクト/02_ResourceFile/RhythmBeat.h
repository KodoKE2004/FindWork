#pragma once

#include <cmath>
#include <cstdint>

struct RhythmBeatConst
{
    // input
    float m_Bpm = 120.0f;
    int   m_BeatsPerBar = 4;      // MainBeat per bar
    int   m_BeatUnit = 4;
    int   m_TicksPerBeat = 16;    // Tick split for 1 MainBeat
    int   m_HitsPerBar = 4;
    int   m_SubBeatsPerBar = 8;   // SubBeat per bar (loop base)

    // derived
    float secondsPerBeat = 0.0f;
    float secondsPerBar = 0.0f;
    float secondsPerTick = 0.0f;
    float ticksPerSecond = 0.0f;

    float secondsPerSubBeat = 0.0f;
    float subBeatsPerSecond = 0.0f;

    float secondsPerHit = 0.0f;
    float hitsPerSecond = 0.0f;

    float subToMain = 0.0f;
    int   subBeatsPerMainBeat = 1;

    void Setup(float bpm = 120.0f,
        int beatsPerBar = 4,
        int beatUnit = 4,
        int ticksPerBeat = 16,
        int hitsPerBar = 4,
        int subBeatsPerBar = 8)
    {
        m_Bpm = bpm;
        m_BeatsPerBar = (beatsPerBar <= 0) ? 1 : beatsPerBar;
        m_BeatUnit = (beatUnit <= 0) ? 4 : beatUnit;
        m_TicksPerBeat = (ticksPerBeat <= 0) ? 1 : ticksPerBeat;
        m_HitsPerBar = (hitsPerBar <= 0) ? 1 : hitsPerBar;
        m_SubBeatsPerBar = (subBeatsPerBar <= 0) ? 1 : subBeatsPerBar;

        const float secondsPerQuarter = 60.0f / m_Bpm;
        secondsPerBeat = secondsPerQuarter * (4.0f / static_cast<float>(m_BeatUnit));
        secondsPerBar = secondsPerBeat * static_cast<float>(m_BeatsPerBar);

        secondsPerTick = secondsPerBeat / static_cast<float>(m_TicksPerBeat);
        ticksPerSecond = 1.0f / secondsPerTick;

        secondsPerHit = secondsPerBar / static_cast<float>(m_HitsPerBar);
        hitsPerSecond = 1.0f / secondsPerHit;

        secondsPerSubBeat = secondsPerBar / static_cast<float>(m_SubBeatsPerBar);
        subBeatsPerSecond = 1.0f / secondsPerSubBeat;

        subToMain = static_cast<float>(m_BeatsPerBar) / static_cast<float>(m_SubBeatsPerBar);
        subBeatsPerMainBeat = m_SubBeatsPerBar / m_BeatsPerBar;
        if (subBeatsPerMainBeat <= 0)
        {
            subBeatsPerMainBeat = 1;
        }
    }
};

class RhythmBeat
{
private:
    RhythmBeatConst m_Beat{};
    double          m_SongTimeSec = 0.0;
    int64_t         m_PrevSubBeat = 0;
    int             m_SubBeatAdvance = 0;
    int             m_MainBeatAdvance = 0;
    int             m_SubBeatElapsed = 0;
    int             m_BeatTotal = 0; // SubBeat total

public:
    RhythmBeat() = default;
    void Initialize(const RhythmBeatConst& config, bool isTimeReset);
    void Initialize(const RhythmBeatConst& config, bool isTimeReset, int limit);
    int Update(float tick);

    void SetTickCounter(float count)
    {
        m_SongTimeSec = static_cast<double>(count);
    }

    void TickCount(float tick)
    {
        m_SongTimeSec += static_cast<double>(tick);
    }

    void SetElapsedBeat(int beats)
    {
        m_SubBeatElapsed = beats;
    }

    void SetBpm(float bpm)
    {
        m_Beat.m_Bpm = bpm;
        m_Beat.Setup(m_Beat.m_Bpm,
            m_Beat.m_BeatsPerBar,
            m_Beat.m_BeatUnit,
            m_Beat.m_TicksPerBeat,
            m_Beat.m_HitsPerBar,
            m_Beat.m_SubBeatsPerBar);
    }

    float GetBpm() const
    {
        return m_Beat.m_Bpm;
    }

    int GetTotalTick() const
    {
        return m_SubBeatElapsed * m_Beat.m_TicksPerBeat;
    }

    int GetBeatIndex() const
    {
        return GetMainBeatElapsed();
    }

    int GetTickInBeat() const
    {
        return 0;
    }

    int GetBeatElapsed() const
    {
        return m_SubBeatElapsed;
    }

    int GetMainBeatElapsed() const
    {
        return m_SubBeatElapsed / m_Beat.subBeatsPerMainBeat;
    }

    int GetBeatTotal() const
    {
        return m_BeatTotal;
    }

    int GetMainBeatTotal() const
    {
        return m_BeatTotal / m_Beat.subBeatsPerMainBeat;
    }

    int GetBeatRest() const
    {
        return m_BeatTotal - m_SubBeatElapsed;
    }

    int GetMainBeatRest() const
    {
        return GetMainBeatTotal() - GetMainBeatElapsed();
    }

    float GetOneBeat() const
    {
        return m_Beat.secondsPerBeat;
    }

    const RhythmBeatConst& GetBeatConst() const
    {
        return m_Beat;
    }

    int GetAdvance() const
    {
        return m_MainBeatAdvance;
    }
};