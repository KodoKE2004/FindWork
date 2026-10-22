#pragma once
#include "RhythmBeat.h"

// 一回のゲームプレイ中に共有する状態をまとめるクラス
// 現段階ではRhythmBeatだけを所有する
class GameplaySession
{
public:
    enum class State
    {
        Stopped,
        Playing,
        Paused
    };
private:
    RhythmBeat m_RhythmBeat;
    State m_State = State::Stopped;
public:
    GameplaySession() = default;
    ~GameplaySession() = default;

    //----------------------------
    //      リズム設定
    //----------------------------
    // 総拍数を変更せず、リズム設定を更新する
    void InitializeRhythm(
        const RhythmBeatConst& config,
        bool isTimeReset);
    //総拍数を指定してリズム設定を更新する
    void InitializeRhythm(
        const RhythmBeatConst& config,
        bool isTimeReset,
        int totalBeats);
    //----------------------------
    //      更新
    //----------------------------
    // Playingのときだけリズムを進める
    RhythmBeatResult Update(float deltaTime);
    // 現在の経過拍数を指定する
    void SetElapsedBeat(int beats);
    //----------------------------
    //      再生状態
    //----------------------------
    void Play();
    void Pause();
    void Stop();
    bool IsPlaying() const;
    State GetState() const;
    //----------------------------
    //      BPM
    //----------------------------
    void SetBpm(float bpm);
    float GetBpm() const;
    //----------------------------
    //      読み取り専用アクセス
    //----------------------------
    const RhythmBeat& GetRhythmBeat() const;
};

