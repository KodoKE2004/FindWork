#include "RhythmBeat.h"

void RhythmBeat::Initialize(const RhythmBeatConst& config, bool isTimeReset)
{
    m_Beat = config;
    m_TickIndex = 0;
    m_BeatElapsed = 0;
    if (isTimeReset) {
        m_TickCounter = 0.0f;
    }
}

void RhythmBeat::Initialize(const RhythmBeatConst& config, bool isTimeReset, int limit)
{
    m_Beat = config;
    m_TickIndex = 0;
    m_BeatTotal = limit;
    m_BeatElapsed = 0;
    if(isTimeReset) {
        m_TickCounter = 0.0f;
    }
}


int RhythmBeat::Update(float tick)
{
    // 経過秒数をTickに変換してカウントアップ
    m_TickCounter += m_Beat.ticksPerSecond * tick;
    m_Advance = 0;
    // 1Tick以上進んでいたらTick数を進める
    while (m_TickCounter >= 1.0f)
    {
        m_TickCounter -= 1.0f;
        ++m_TickIndex;

        if ((m_TickIndex % m_Beat.m_TicksPerBeat) == 0)
        {
            ++m_Advance;
            ++m_BeatElapsed;
        }
    }

    return m_Advance;
}
