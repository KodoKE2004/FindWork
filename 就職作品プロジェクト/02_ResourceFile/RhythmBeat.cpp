#include "RhythmBeat.h"

void RhythmBeat::Initialize(const RhythmBeatConst& config)
{
    m_Beat = config;
    m_TickCounter = 0.0f;
    m_TickIndex = 0;
}

int RhythmBeat::Update(float deltaTime)
{
    // 1秒あたり ticksPerSecond 進むので、deltaSecondsを掛けて進行
    m_TickCounter += m_Beat.ticksPerSecond * deltaTime;

    int advanced = 0; // このフレームで何Tick進んだか

    while (m_TickCounter >= 1.0f)
    {
        m_TickCounter -= 1.0f;
        ++m_TickIndex;
        ++advanced;
    }

    return advanced;
}