#include "GameplaySession.h"

void GameplaySession::InitializeRhythm(const RhythmBeatConst& config,
                                       bool isTimeReset)
{
    m_RhythmBeat.Initialize(config,
                            isTimeReset);
}

void GameplaySession::InitializeRhythm(const RhythmBeatConst& config,
                                       bool isTimeReset,
                                       int totalBeats)
{
    m_RhythmBeat.Initialize(config,
                            isTimeReset,
                            totalBeats);
}

RhythmBeatResult GameplaySession::Update(float deltaTime)
{
    if (m_State == State::Playing)
    {
        return m_RhythmBeat.Update(deltaTime);
    }
    // í‚é~íÜÅEàÍéûí‚é~íÜÇÕîèÇêiÇﬂÇ»Ç¢
    const int currentBeat = m_RhythmBeat.GetBeatElapsed();
    RhythmBeatResult result{};
    result.previousBeat = currentBeat;
    result.currentBeat = currentBeat;
    result.advancedBeatCount = 0;
    return result;
}

void GameplaySession::SetElapsedBeat(int beats)
{
    m_RhythmBeat.SetElapsedBeat(beats);
}

void GameplaySession::Play()
{
    m_State = State::Playing;
}

void GameplaySession::Pause()
{
    if (m_State == State::Playing)
    {
        m_State = State::Paused;
    }
}

void GameplaySession::Stop()
{
    m_State = State::Stopped;
}

bool GameplaySession::IsPlaying() const
{
    return m_State == State::Playing;
}

GameplaySession::State GameplaySession::GetState() const
{
    return m_State;
}

void GameplaySession::SetBpm(float bpm)
{
    m_RhythmBeat.SetBpm(bpm);
}

float GameplaySession::GetBpm() const
{
    return m_RhythmBeat.GetBpm();
}

const RhythmBeat& GameplaySession::GetRhythmBeat() const
{
    return m_RhythmBeat;
}