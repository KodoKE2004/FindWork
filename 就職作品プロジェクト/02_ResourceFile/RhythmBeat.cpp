#include "RhythmBeat.h"

void RhythmBeat::Initialize(const RhythmBeatConst& config, bool isTimeReset)
{
    m_Beat            = config;
    m_SubBeatAdvance  = 0;
    m_MainBeatAdvance = 0;
    m_SubBeatElapsed  = 0;
    if (isTimeReset)
    {
        m_SongTimeSec = 0.0;
    }
    m_PrevSubBeat = 0;
}

void RhythmBeat::Initialize(const RhythmBeatConst& config, bool isTimeReset, int limit)
{
    m_Beat            = config;
    m_SubBeatAdvance  = 0;
    m_MainBeatAdvance = 0;
    m_SubBeatElapsed  = 0;
    m_BeatTotal       = limit;
    if (isTimeReset)
    {
        m_SongTimeSec = 0.0;
    }
    m_PrevSubBeat = 0;
}

int RhythmBeat::Update(float tick)
{
    m_SongTimeSec += static_cast<double>(tick);
    m_SubBeatAdvance = 0;
    m_MainBeatAdvance = 0;

    if (m_Beat.secondsPerSubBeat <= 0.0f)
    {
        return 0;
    }

    const int64_t curSubBeat = static_cast<int64_t>(std::floor(m_SongTimeSec / static_cast<double>(m_Beat.secondsPerSubBeat)));

    while (m_PrevSubBeat < curSubBeat)
    {
        ++m_PrevSubBeat;
        ++m_SubBeatAdvance;
        ++m_SubBeatElapsed;

        if ((m_PrevSubBeat % m_Beat.subBeatsPerMainBeat) == 0)
        {
            ++m_MainBeatAdvance;
        }
    }

    return m_MainBeatAdvance;
}