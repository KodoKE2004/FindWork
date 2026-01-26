#include "RhythmBeat.h"

void RhythmBeat::Initialize(const RhythmBeatConst& config)
{
    m_Beat = config;
    m_TickCounter = 0.0f;
    m_TickIndex = 0;
}

int RhythmBeat::Update(float deltaTime)
{
    // 経過秒数をTickに変換してカウントアップ
    m_TickCounter += m_Beat.ticksPerSecond * deltaTime;
    // このフレームで何Tick進んだか
    m_Advance = 0; 
    // 1Tick以上進んでいたらTick数を進める
    while (m_TickCounter >= 1.0f)
    {
        m_TickCounter -= 1.0f;
        ++m_TickIndex;
        ++m_Advance;
    }

    return m_Advance;
}