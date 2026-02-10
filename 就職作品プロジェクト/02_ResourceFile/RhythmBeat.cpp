#include "RhythmBeat.h"

void RhythmBeat::Initialize(const RhythmBeatConst& config)
{
    m_Beat = config;
    m_TickCounter = 0.0f;
    m_TickIndex = 0;
}

void RhythmBeat::Initialize(const RhythmBeatConst& config, int limit)
{
    m_Beat = config;
    m_TickCounter = 0.0f;
    m_TickIndex = 0;
    m_BeatTotal = limit;
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
        ++m_Advance;
        ++m_BeatElapsed;
    }

    return m_Advance;
}
