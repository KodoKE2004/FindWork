#pragma once
#include <xaudio2.h>
#pragma comment(lib, "xaudio2.lib")
#include <vector>
#include <atomic>
#include <memory>
#include <cstdint>

// ------------------- WAV clip -------------------
struct AudioClip {
	WAVEFORMATEX* wfex = nullptr; // 所有
	std::vector<uint8_t> pcm; // 生データ
	~AudioClip() { delete wfex; }
};


//======================================
// 			  ループ設定
//======================================
struct LoopConfig {
	uint32_t loopBegin = 0; // samples
	uint32_t loopLength = 0; // 0=to end
	uint32_t loopCount = 0; // 0=no loop / XAUDIO2_LOOP_INFINITE
};

//======================================
//			 再生パラメータ
//======================================
struct PlayParams {
	float volume = 1.0f; // 0..1+
	float pitch = 1.0f; // frequency ratio
	float pan = 0.0f; // -1..+1 (stereo only)
	LoopConfig loop{};
	bool autoRelease = true; // 再生終了後に自動回収
};

//=====================================
//	    	  コールバック
//=====================================
struct VoiceCallback : public IXAudio2VoiceCallback {
	std::atomic<bool>* finished = nullptr;
	void OnStreamEnd() noexcept override { if (finished) finished->store(true, std::memory_order_release); }
	void OnVoiceError(void*, HRESULT) noexcept override { if (finished) finished->store(true, std::memory_order_release); }
	void OnVoiceProcessingPassStart(UINT32) noexcept override {}
	void OnVoiceProcessingPassEnd() noexcept override {}
	void OnBufferStart(void*) noexcept override {}
	void OnBufferEnd(void*) noexcept override {}
	void OnLoopEnd(void*) noexcept override {}
};

class Audio
{
private:
	void submitBuffer(const LoopConfig& loop);
	void applyPan(float pan);

	IXAudio2* m_xa = nullptr; // 借用
	IXAudio2MasteringVoice* m_master = nullptr; // 借用
	IXAudio2SourceVoice* m_voice = nullptr; // 所有
	std::shared_ptr<AudioClip> m_clip;
	VoiceCallback m_cb{};
	std::atomic<bool> m_finished{ false };
	bool m_autoRelease = true;
	uint32_t m_id = 0;

public:
	Audio(IXAudio2* xa, IXAudio2MasteringVoice* master, std::shared_ptr<AudioClip> clip, uint32_t id);
	~Audio();

	uint32_t Id() const { return m_id; }
	bool IsFinished() const { return m_finished.load(std::memory_order_acquire); }
	bool IsPlaying() const;

	void Play(const PlayParams& params);
	void Stop(bool immediate = true);
	void SetVolume(float v);
	void SetPitch(float ratio);
	void SetPan(float pan);

	IXAudio2SourceVoice* Raw() const { return m_voice; }
	const std::shared_ptr<AudioClip>& Clip() const { return m_clip; }
	bool AutoRelease() const { return m_autoRelease; }
	void SetAutoRelease(bool b) { m_autoRelease = b; }
};