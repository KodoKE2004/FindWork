#pragma once
#include <xaudio2.h>
#pragma comment(lib, "xaudio2.lib")

#include <string>
#include <vector>
#include <atomic>
#include <memory>
#include <cstdint>

// ------------------- WAV clip -------------------
struct AudioClip {
	WAVEFORMATEX* wfex = nullptr;	// 所有
	std::vector<uint8_t> pcm;		// 生データ
	~AudioClip() { delete wfex; }
};


//======================================
// 			  ループ設定
//======================================
struct LoopConfig {
	uint32_t loopBegin = 0;  // samples
	uint32_t loopLength = 0; // 0=to end
	uint32_t loopCount = 0;  // 0=no loop / XAUDIO2_LOOP_INFINITE
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

struct AudioConfig
{
	std::wstring filePath;
	PlayParams params;
	bool loop = false;
	bool autoPlay = false;
};


//=====================================
//	    	  コールバック
//=====================================
struct VoiceCallback : public IXAudio2VoiceCallback {
    // 再生終了を検知するために利用
	std::atomic<bool>* finished = nullptr;
    // IXAudio2VoiceCallback
	void OnStreamEnd() noexcept override { if (finished) finished->store(true, std::memory_order_release); }
    // エラー発生時にも再生終了とみなす
	void OnVoiceError(void*, HRESULT) noexcept override { if (finished) finished->store(true, std::memory_order_release); }
    // 未使用
	void OnVoiceProcessingPassStart(UINT32) noexcept override {}
    // 未使用
	void OnVoiceProcessingPassEnd() noexcept override {}
    // 未使用
	void OnBufferStart(void*) noexcept override {}
    // 未使用
	void OnBufferEnd(void*) noexcept override {}
    // 未使用
	void OnLoopEnd(void*) noexcept override {}
};

class Audio
{
private:
    // 内部関数
	void submitBuffer(const LoopConfig& loop);
    // panの適用
	void applyPan(float pan);

    IXAudio2* m_xa = nullptr;					// 借用
	IXAudio2MasteringVoice* m_master = nullptr; // 借用
	IXAudio2SourceVoice* m_voice = nullptr;		// 所有
    std::shared_ptr<AudioClip> m_clip;			// 所有
    VoiceCallback m_cb{};						// 所有
    std::atomic<bool> m_finished{ false };		// 再生終了検知用
    bool m_autoRelease = true;					// 再生終了後に自動回収
    uint32_t m_id = 0;							// 識別用

public:
	Audio(IXAudio2* xa, IXAudio2MasteringVoice* master, std::shared_ptr<AudioClip> clip, uint32_t id);
	~Audio();

    // コピー禁止 
	uint32_t Id() const { return m_id; }
    // ムーブ禁止 
	bool IsFinished() const { return m_finished.load(std::memory_order_acquire); }
    // 再生中かどうか
	bool IsPlaying() const;
    // 再生
	void Play(const PlayParams& params);
    // 停止
	void Stop(bool immediate = true);
    // パラメータ設定
	void SetVolume(float v);
    // ピッチ設定
	void SetPitch(float ratio);
    // パン設定
	void SetPan(float pan);
    // 生データの長さ(サンプル数)
	IXAudio2SourceVoice* Raw() const { return m_voice; }
    // クリップ情報
	const std::shared_ptr<AudioClip>& Clip() const { return m_clip; }
    // 自動回収設定
	bool AutoRelease() const { return m_autoRelease; }
    // 再生終了後に自動回収
	void SetAutoRelease(bool b) { m_autoRelease = b; }
};