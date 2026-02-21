#pragma once

// ƒŠƒYƒ€‚Ì•Ï”
struct RhythmBeatConst
{
<<<<<<< HEAD
    // input
    float m_Bpm = 120.0f;
    int   m_BeatsPerBar = 8;      // MainBeat per bar
    int   m_BeatUnit = 8;
    int   m_TicksPerBeat = 16;    // Tick split for 1 MainBeat
    int   m_HitsPerBar = 8;
    int   m_SubBeatsPerBar = 8;   // SubBeat per bar (loop base)
=======
    // “ü—Í
    float m_Bpm = 120.0f;          // l•ª‰¹•„Šî€‚ÌBPM
    int   m_BeatsPerBar = 4;       // •ªqF¬ß‚Ì””
    int   m_BeatUnit = 4;          // •ª•êF4=l•ª, 8=”ª•ª...
    int   m_TicksPerBeat = 16;     // 1”‚ğ‰½•ªŠ„‚·‚é‚©
    int   m_HitsPerBar = 4;        // š1¬ß‚É‰½‰ñ’@‚­(ƒCƒxƒ“ƒg)‚©i•Ï‘¥OKj
>>>>>>> parent of 6c38f02 (é€²æ—ãªã—)

    // ŒvZŒ‹‰Ê
    float secondsPerBeat = 0.0f;   // 1”i•ª•ê‚Ì‰¹•„j‚Ì•b
    float secondsPerBar  = 0.0f;   // 1¬ß‚Ì•b
    float secondsPerTick = 0.0f;   // 1Tick‚Ì•b
    float ticksPerSecond = 0.0f;   // 1•b‚ ‚½‚èTick

    float secondsPerHit = 0.0f;   // š1‰ñ’@‚­ŠÔŠui¬ß‚ğ hitsPerBar ‚ÅŠ„‚éj
    float hitsPerSecond = 0.0f;   // š1•b‚ ‚½‚è’@‚­‰ñ”

<<<<<<< HEAD
    void Setup(float bpm   = 120.0f,
        int beatsPerBar    = 8,
        int beatUnit       = 8,
        int ticksPerBeat   = 16,
        int hitsPerBar     = 8,
        int subBeatsPerBar = 8)
    {
        m_Bpm = bpm;
        m_BeatsPerBar    = (beatsPerBar    <= 0) ? 1 : beatsPerBar;
        m_BeatUnit       = (beatUnit       <= 0) ? 4 : beatUnit;
        m_TicksPerBeat   = (ticksPerBeat   <= 0) ? 1 : ticksPerBeat;
        m_HitsPerBar     = (hitsPerBar     <= 0) ? 1 : hitsPerBar;
        m_SubBeatsPerBar = (subBeatsPerBar <= 0) ? 1 : subBeatsPerBar;
=======
    void Setup(float bpm = 120.0f,
        int beatsPerBar  = 8,
        int beatUnit     = 8,
        int ticksPerBeat = 16,
        int hitsPerBar   = 8)
    {
        m_Bpm = bpm;
        m_BeatsPerBar   = beatsPerBar;
        m_BeatUnit      = beatUnit;
        m_TicksPerBeat  = ticksPerBeat;
        m_HitsPerBar    = (hitsPerBar <= 0) ? 1 : hitsPerBar; // 0œZ–h~
>>>>>>> parent of 6c38f02 (é€²æ—ãªã—)

        // l•ª‰¹•„‚Ì•b
        const float secondsPerQuarter = 60.0f / m_Bpm;

        // 1”i•ª•ê‰¹•„j‚Ì•bF4¨8‚È‚ç”¼•ª
        secondsPerBeat = secondsPerQuarter * (4.0f / static_cast<float>(m_BeatUnit));

        // ¬ß•bF””i•ªqj~1”•b
        secondsPerBar = secondsPerBeat * static_cast<float>(m_BeatsPerBar);

        // Tick•b
        secondsPerTick = secondsPerBeat / static_cast<float>(m_TicksPerBeat);
        ticksPerSecond = 1.0f / secondsPerTick;

        // š’@‚­ŠÔŠui¬ß‚ğ hits ‚ÅŠ„‚éj
        secondsPerHit = secondsPerBar / static_cast<float>(m_HitsPerBar);
        hitsPerSecond = 1.0f / secondsPerHit;
    }
};
class RhythmBeat
{
private:
    RhythmBeatConst m_Beat{};
    float           m_TickCounter = 0.0f;   // Œo‰ßŠÔ‚Ì’~Ï
    int             m_TickIndex   = 0;      // Œ»İ‚ÌTick”
    int             m_Advance     = 0;      // Œ»İ‚Ì”qƒCƒ“ƒfƒbƒNƒX
    int             m_BeatElapsed = 0;      // Œo‰ß””
    int             m_BeatTotal   = 0;      // ‘Tick”

public:
    RhythmBeat() = default;
    // ‰Šú‰»
    void Initialize(const RhythmBeatConst& config, bool isTimeReset);
    void Initialize(const RhythmBeatConst& config, bool isTimeReset, int limit);
    // Œo‰ß””‚Ìæ“¾‚ğs‚¤
    int Update(float tick);

    void SetTickCounter(float count)
    {
        m_TickCounter = count;
    }

    void TickCount(float tick) {
        m_TickCounter += tick;
    }

    void SetElapsedBeat(int beats)
    {
<<<<<<< HEAD
        m_SubBeatElapsed = beats * m_Beat.subBeatsPerMainBeat;
        m_PrevSubBeat = m_SubBeatElapsed;
=======
        m_BeatElapsed = beats;
>>>>>>> parent of 6c38f02 (é€²æ—ãªã—)
    }

    void SetBpm(float bpm)
    {
        m_Beat.m_Bpm = bpm;
        // BPM•ÏX‚É”º‚¢AŠÖ˜A‚·‚éŠÔŒvZ‚ğXV
        m_Beat.Setup(m_Beat.m_Bpm, m_Beat.m_BeatUnit, m_Beat.m_TicksPerBeat);
    }

    float GetBpm() const
    {
        return m_Beat.m_Bpm;
    }

    // Œ»İ‚ÌTick‚ğæ“¾
    int GetTotalTick() const
    {
<<<<<<< HEAD
        return GetMainBeatElapsed() * m_Beat.m_TicksPerBeat;
=======
        return m_TickIndex;
>>>>>>> parent of 6c38f02 (é€²æ—ãªã—)
    }

    // Œ»İ‚Ì”qƒCƒ“ƒfƒbƒNƒX‚ğæ“¾
    // ¡‚ª‰½”‘–Ú‚©‚ğ•Ô‚·
    int GetBeatIndex() const
    {
        return m_TickIndex / m_Beat.m_TicksPerBeat;
    }

    // Œ»İ‚Ì”q“à‚ÌTickˆÊ’u‚ğæ“¾
    // ¡‚Ì”q‚Ì’†‚Å‰½Tick–Ú‚©‚ğ•Ô‚·
    int GetTickInBeat() const
    {
        return m_TickIndex % m_Beat.m_TicksPerBeat;
    }

    // Œo‰ß””‚ğæ“¾
    int GetBeatElapsed() const
    {
<<<<<<< HEAD
        return GetMainBeatElapsed();
    }

    int GetMainBeatElapsed() const
    {
        return m_SubBeatElapsed / m_Beat.subBeatsPerMainBeat;
=======
        return m_BeatElapsed;
>>>>>>> parent of 6c38f02 (é€²æ—ãªã—)
    }

    // ‘””‚ğæ“¾
    int GetBeatTotal() const
    {
        return GetMainBeatTotal();
    }

    // c””‚ğæ“¾
    int GetBeatRest() const
    {
<<<<<<< HEAD
        return GetMainBeatRest();
=======
        return m_BeatTotal - m_BeatElapsed;
>>>>>>> parent of 6c38f02 (é€²æ—ãªã—)
    }

    // ˆê”‚Ì’·‚³‚ğæ“¾
    float GetOneBeat() const 
    {
        return m_Beat.secondsPerBeat;
    }

    const RhythmBeatConst& GetBeatConst()const
    {
        return m_Beat;
    }

    int GetAdvance() const {
        return m_Advance;
    }
};
