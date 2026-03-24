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
    // 秒を Tick に変換して蓄積する。小数分を残すことでフレーム依存の誤差を抑える。
    m_TickCounter += m_Beat.ticksPerSecond * tick;
    m_Advance = 0;

    // 1フレームで複数 Tick 進む可能性があるため while で消化する。
    while (m_TickCounter >= 1.0f)
    {
        m_TickCounter -= 1.0f;
        ++m_TickIndex;

        // 1拍境界に到達したタイミングだけ拍進行として扱う。
        if ((m_TickIndex % m_Beat.m_TicksPerBeat) == 0)
        {
            ++m_Advance;
            ++m_BeatElapsed;
        }
    }

    return m_Advance;
}
