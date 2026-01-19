#pragma once
#include <xaudio2.h>
#pragma comment(lib, "xaudio2.lib")

#include <string>
#include <vector>
#include <atomic>
#include <memory>
#include <cstdint>
#include <algorithm>
#include <cmath>
#include "DirectXMath.h"

#ifdef _DEBUG
#include "../03_ExternalFile/imgui/imgui.h"
#include "DebugUI.h"
#endif

// ------------------- WAV clip -------------------
struct AudioClip {
	WAVEFORMATEX* wfex = nullptr;	// 所有
	std::vector<uint8_t> pcm;		// 生データ
	~AudioClip() { delete[] reinterpret_cast<uint8_t*>(wfex); }
};


//-------------------------------
// 			  ループ設定
//-------------------------------
struct LoopConfig {
	uint32_t loopBegin  = 0;  // samples
	uint32_t loopLength = 0;  // 0=to end
	uint32_t loopCount  = 0;  // 0=no loop / XAUDIO2_LOOP_INFINITE
};

//-------------------------------
//			 再生パラメータ
//-------------------------------
struct PlayParams {
	float volume = 1.0f;	// 0..1+
	float pitch = 1.0f;		// frequency ratio
	float pan = 0.0f;		// -1..+1 (stereo only)
	LoopConfig loop{};
	bool autoRelease = true; // 再生終了後に自動回収
};

struct AudioConfig
{
    std::wstring filePath;	// WAVファイルパス
    PlayParams params;		// 再生パラメータ
    bool loop = false;		// ループ再生するか
	bool autoPlay = false;	// 登録時に自動再生するか
    float bpm = 120.0f;		// 曲のBPM（必要なら）
    float baseBpm = 120.0f; // 基準BPM（必要なら）
};

// フレーム単位で渡す「傾き」
struct SlopePerSec {
    float volDbPerSec		= 0.0f;		// volume
    float pitchSemiPerSec	= 0.0f;		// pitch
    float panPerSec			= 0.0f;		// pan
};

// 再生ランタイム状態（フレーム間で持ち越す）
struct VoiceState {
	double Fs	 = 48000.0;
	double phase = 0.0;           // 発振や再サンプリングの位相など
	double sampleRemainder = 0.0; // Nが非整数の端数を持ち越し

	// 補間は「耳に近い軸」で持つ
	float volDb = 0.0f;  // いまの音量[dB]
	float pitchSemi = 0.0f;  // いまのピッチ[半音]
	float pan = 0.0f;  // いまのパン[-1..+1]
};

// 変換ヘルパ関数
inline float DbToLin	(float db)    { return powf(10.0f, db  / 20.0f) ; }
inline float SemiToRatio(float semi)  { return powf(2.0f, semi / 12.0f) ; }
inline float LinToDb	(float lin)   { return 20.0f * log10f(max(lin  , 1e-6f)); }
inline float RatioToSemi(float ratio) { return 12.0f * log2f (max(ratio, 1e-6f)); }
inline float SlopeToHitInFrames(float current, float target, int framesAt60)
{
    if(framesAt60 <= 0) return 0.0f;
    return 60.0f * (target - current) / (float)framesAt60;
}

//-------------------------------====
//	    	  コールバック
//-------------------------------====
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


//-------------------------------====
//				Audioクラス
//-------------------------------====
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
    float m_Bpm   = 120.0f;						// 曲のBPM（必要なら）
    float m_BaseBpm = 120.0f;					// 基準BPM（必要なら）
public:
	Audio() = default;
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
	void SetBpm(float bpm);
	float GetBpm() const { return m_Bpm; }
    float GetBaseBpm() const{return m_BaseBpm;};
};


//-------------------------------====
//			傾きを求める
//-------------------------------====
static void RenderFrameSlope(
    VoiceState& st,
    const SlopePerSec& slope,   // このフレームでの傾き（毎秒）
    double dtFrame,             // 1/60 など
    std::vector<float>& outL,
    std::vector<float>& outR,
    double baseFreqHz)          // 基準周波数（ピッチ比で変化）
{
    // このフレームで必要なサンプル数
    double exact = dtFrame * st.Fs + st.sampleRemainder;
    int    N = (int)floor(exact);
    st.sampleRemainder = exact - N;
    if (N <= 0) return;

    size_t base = outL.size();
    outL.resize(base + N);
    outR.resize(base + N);

    // サンプルあたりの増分（傾き/秒 ÷ Fs）
    const float dVolDb      = slope.volDbPerSec     / (float)st.Fs ;
    const float dPitchSemi  = slope.pitchSemiPerSec / (float)st.Fs ;
    const float dPan        = slope.panPerSec       / (float)st.Fs ;

    for (int n = 0; n < N; ++n) {
        // 積分（Integrate）
        st.volDb += dVolDb;
        st.pitchSemi += dPitchSemi;
        st.pan = std::clamp(st.pan + dPan, -1.0f, 1.0f);

        const float amp = DbToLin(st.volDb);
        const float ratio = SemiToRatio(st.pitchSemi);
        const double f = baseFreqHz * ratio;

        // 位相更新
        const double dphi = 2.0 * DirectX::XM_PI * f / st.Fs;
        st.phase += dphi;
        if (st.phase >= 2.0 * DirectX::XM_PI) st.phase -= 2.0 * DirectX::XM_PI;

        const float s = sinf((float)st.phase) * amp;

        // パン（定電力）: pan -1..+1 → 0..1
        const float p = (st.pan * 0.5f + 0.5f);
        const float gL = cosf(p * float(DirectX::XM_PI) * 0.5f);
        const float gR = sinf(p * float(DirectX::XM_PI) * 0.5f);

        outL[base + n] = s * gL;
        outR[base + n] = s * gR;
    }
}



#ifdef _DEBUG
namespace
{
	class AudioHelperTickDebugger
	{
	public:
		AudioHelperTickDebugger()
		{
			m_state.Fs = 48000.0;
			m_state.volDb = LinToDb(0.2f);
			m_state.pan = 0.0f;
			m_state.pitchSemi = 0.0f;
		}

		void Tick(float dt)
		{
			if (dt <= 0.0f) { return; }

			m_elapsed += dt;

			const float targetVolDb		= sinf(m_elapsed * 0.5f) * -6.0f;
			const float targetPitchSem	= sinf(m_elapsed * 0.25f) * 2.0f;
			const float targetPan		= sinf(m_elapsed * 0.15f);

			m_slope.volDbPerSec		= SlopeToHitInFrames(m_state.volDb, targetVolDb, 45);
			m_slope.pitchSemiPerSec = SlopeToHitInFrames(m_state.pitchSemi, targetPitchSem, 45);
			m_slope.panPerSec		= SlopeToHitInFrames(m_state.pan, targetPan, 45);

			m_waveL.clear();
			m_waveR.clear();
			RenderFrameSlope(m_state, m_slope, dt, m_waveL, m_waveR, 440.0);
		}

		void DrawUI()
		{
			if (ImGui::Begin("Audio Helper Debug"))
			{
				ImGui::Text("Elapsed : %.3f s", m_elapsed);
				ImGui::Text("Volume  : %.2f dB", m_state.volDb);
				ImGui::Text("Pitch   : %.2f semi", m_state.pitchSemi);
				ImGui::Text("Pan     : %.2f", m_state.pan);

				if (!m_waveL.empty())
				{
					ImGui::Separator();
					ImGui::Text("Left Channel");
					ImGui::PlotLines("", m_waveL.data(), static_cast<int>(m_waveL.size()), 0, nullptr, -1.0f, 1.0f, ImVec2(0, 80));
				}
				if (!m_waveR.empty())
				{
					ImGui::Separator();
					ImGui::Text("Right Channel");
					ImGui::PlotLines(" ", m_waveR.data(), static_cast<int>(m_waveR.size()), 0, nullptr, -1.0f, 1.0f, ImVec2(0, 80));
				}
			}
			ImGui::End();
		}

	private:
		VoiceState m_state{};
		SlopePerSec m_slope{};
		float m_elapsed = 0.0f;
		std::vector<float> m_waveL;
		std::vector<float> m_waveR;
	};

	AudioHelperTickDebugger& GetAudioHelperTickDebugger()
	{
		static AudioHelperTickDebugger instance;
		return instance;
	}

	void RegisterAudioHelperDebugUI()
	{
		static bool registered = false;
		if (registered) { return; }
		registered = true;
		DebugUI::RedistDebugFunction([]() { GetAudioHelperTickDebugger().DrawUI(); });
	}
}
#endif // _DEBUG
