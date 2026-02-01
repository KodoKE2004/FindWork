#include "Audio.h"
#include "Debug.hpp"

Audio::Audio(IXAudio2* xa, IXAudio2MasteringVoice* master,
	std::shared_ptr<AudioClip> clip, uint32_t id)
	: m_xa(xa), m_master(master), m_clip(std::move(clip)), m_id(id)
{
	m_cb.finished = &m_finished;
    // ソースボイスの作成 XAUDIO2_DEFAULT_FREQ_RATIO = 1.0f +-1.0fの範囲でピッチ変更可能
	HRESULT hr = m_xa->CreateSourceVoice(&m_voice, m_clip->wfex, 0, XAUDIO2_DEFAULT_FREQ_RATIO, &m_cb);
	if (FAILED(hr)) throw std::runtime_error("CreateSourceVoice failed");
}


Audio::~Audio() {
	if (m_voice) {
		m_voice->Stop(0);
		m_voice->FlushSourceBuffers();
		m_voice->DestroyVoice();
		m_voice = nullptr;
	}
}


void Audio::submitBuffer(const LoopConfig& loop) {
	XAUDIO2_BUFFER buf{};
	buf.Flags = XAUDIO2_END_OF_STREAM;
	buf.pAudioData = m_clip->pcm.data();
	buf.AudioBytes = (UINT32)m_clip->pcm.size();
	if (loop.loopCount > 0 || loop.loopCount == XAUDIO2_LOOP_INFINITE) {
		buf.LoopBegin = loop.loopBegin;
		buf.LoopLength = loop.loopLength;
		buf.LoopCount = loop.loopCount;
	}
	HRESULT hr = m_voice->SubmitSourceBuffer(&buf);
	if (FAILED(hr)) throw std::runtime_error("SubmitSourceBuffer failed");
}


void Audio::Play(const PlayParams& params) {
	m_autoRelease = params.autoRelease;
	m_voice->Stop(0);
    m_voice->FlushSourceBuffers();	// 念のため
    submitBuffer(params.loop);		// ループ設定もここで
    SetVolume(params.volume);		// 0.0f 以上
    SetPitch(params.pitch);			// 未実装
    SetPan(params.pan);				// -1.0f .. +1.0f
	m_finished.store(false, std::memory_order_release);
	m_voice->Start(0);
}


void Audio::Stop(bool immediate) {
	if (!m_voice) return;
	if (immediate) {
		m_voice->Stop(0);
		m_voice->FlushSourceBuffers();
		m_finished.store(true, std::memory_order_release);
	}
	else {
		// 自然停止: 何もしない
	}
}


bool Audio::IsPlaying() const {
	if (!m_voice) return false;
	XAUDIO2_VOICE_STATE st{};
	m_voice->GetState(&st);
	return st.BuffersQueued > 0 && !IsFinished();
}


void Audio::SetVolume(float v) {
	if (m_voice) m_voice->SetVolume(max(0.0f, v));
}


void Audio::SetPitch(float ratio) {
	if (!m_voice) return;
	ratio = std::clamp(ratio, XAUDIO2_MIN_FREQ_RATIO, XAUDIO2_MAX_FREQ_RATIO);
	m_voice->SetFrequencyRatio(ratio);
}

void Audio::applyPan(float pan) {
	if (!m_voice || !m_master) return;
	XAUDIO2_VOICE_DETAILS dst{}; m_master->GetVoiceDetails(&dst);
	if (dst.InputChannels != 2) return; // ステレオのみ
	pan = std::clamp(pan, -1.0f, 1.0f);
	constexpr float PI = 3.14159265358979323846f;
	float theta = (pan + 1.0f) * 0.25f * PI; // [-1..+1] -> [0..π/2]
	float L = std::cos(theta);
	float R = std::sin(theta);


	// Mono -> Stereo / Stereo -> Stereo の両方に対応
	XAUDIO2_VOICE_DETAILS src{}; m_voice->GetVoiceDetails(&src);
	if (src.InputChannels == 1) {
		float mtx[2] = { L, R };
		m_voice->SetOutputMatrix(nullptr, 1, 2, mtx);
	}
	else if (src.InputChannels == 2) {
		float mtx[4] = { L, 0, 0, R }; // Lout<-Lin, Rout<-Rin
		m_voice->SetOutputMatrix(nullptr, 2, 2, mtx);
	}
}


void Audio::SetPan(float pan) 
{
	applyPan(pan); 
}

void Audio::SetBpm(float bpm)
{
	if (bpm <= 0.0f) {
		return;
	}
	m_Bpm = bpm;
	if (m_BaseBpm > 0.0f) {
		SetPitch(m_Bpm / m_BaseBpm);
	}
}
