#pragma once
#include "Audio.h"

#include <wrl/client.h>
#include <unordered_map>
#include <string>
#include <mutex>
#include <filesystem>
#include <type_traits>
using namespace Microsoft::WRL;



class AudioManager
{
public:
	// baseDir: 例 L"assets/audio/"（末尾に自動で区切り追加）
	explicit AudioManager(std::wstring baseDir = L"");
	~AudioManager() { Shutdown(); }

	bool Init();
	void Shutdown();
	void Update(); // 終了インスタンスのGC

	// 登録（アセット）
	bool Add(std::string key, std::wstring relativePath); // 名前登録
	bool Add(int id, std::wstring relativePath); // 数値ID登録（内部で名前に変換）

	// 既存クリップに別名/別IDを付けたい場合
	bool Alias(std::string aliasKey, std::string existingKey);
	bool Alias(int id, std::string existingKey);

	// --- 再生 ---
	uint32_t Play(std::string key, const PlayParams& p = {});
	uint32_t Play(int id, const PlayParams& p = {});

	template<class Enum,
		std::enable_if_t<std::is_enum_v<Enum>, int> = 0>
	uint32_t Play(Enum e, const PlayParams& p = {}) { return Play((int)e, p); }

	// --- 停止/制御 ---
	void Stop(uint32_t handle, bool immediate = true);
	void StopAllByName(std::string key, bool immediate = true);
	void StopAllById(int id, bool immediate = true);

	void SetVolume(uint32_t handle, float v);
	void SetPitch(uint32_t handle, float ratio);
	void SetPan(uint32_t handle, float pan);

	// 便利系
	void StopAll(bool immediate = true);
	// クリップ取得（必要なら）
	std::shared_ptr<AudioClip> GetClip(std::string_view key);

private:
	// 低レベルI/O
	std::shared_ptr<AudioClip> loadWavFull(const std::wstring& fullPath, std::string* err = nullptr);

	// パス結合
	std::wstring joinPath(const std::wstring& base, const std::wstring& rel) const;

	// 検索/内部
	Audio* find(uint32_t id);

private:
	// XAudio2
	ComPtr<IXAudio2> m_xa;
	IXAudio2MasteringVoice* m_master = nullptr;


	// 登録
	std::wstring m_baseDir; // 共通基準パス
	std::unordered_map<std::string, std::shared_ptr<AudioClip>> m_key2clip; // name -> clip
	std::unordered_map<int, std::string> m_id2key; // id -> name


	// 実行中インスタンス
	std::mutex mtx_;
	std::vector<std::unique_ptr<Audio>> m_instances;
	uint32_t m_nextId = 1;
};

