#include "AudioManager.h"
#include <fstream>
#include <filesystem>
#include <algorithm>
#include <cstring>
#include <cmath>
#include <stdexcept>
#include <mmreg.h>

// ========================= AudioManager =========================

AudioManager& AudioManager::Get()
{
    // TODO: return ステートメントをここに挿入します
    static AudioManager instance;
    return instance;
}

AudioManager::AudioManager(std::wstring baseDir) : m_baseDir(std::move(baseDir)) 
{
    if (!m_baseDir.empty()) {
        wchar_t last = m_baseDir.back();
        if (last != L'/' && last != L'\\') m_baseDir.push_back(L'/');
    }
}

bool AudioManager::Initialize() 
{
    if (m_xa) return true;
    HRESULT hr = XAudio2Create(m_xa.GetAddressOf(), 0, XAUDIO2_DEFAULT_PROCESSOR);
    if (!CHECK_HR(hr, "XAudio2Create")) {
        return false;
    }
    hr = m_xa->CreateMasteringVoice(&m_master);
    if (!CHECK_HR(hr, "IXAudio2::CreateMasteringVoice")) {
        m_xa.Reset();
        return false;
    }
    return true;
}

void AudioManager::Finalize() 
{
    std::lock_guard<std::mutex> lk(mtx_);
    m_instances.clear();
    m_key2clip.clear();
    m_id2key.clear();
    m_PathCache.clear();
    if (m_master) { m_master->DestroyVoice(); m_master = nullptr; }
    m_xa.Reset();
}

void AudioManager::Update() 
{
    std::lock_guard<std::mutex> lk(mtx_);
    m_instances.erase(
        std::remove_if(m_instances.begin(), m_instances.end(),
            [](const std::unique_ptr<Audio>& p) { return p->AutoRelease() && p->IsFinished(); }),
        m_instances.end());
}

std::wstring AudioManager::joinPath(const std::wstring& base, const std::wstring& rel) const 
{
    if (rel.empty()) return base;
    if (rel.front() == L'/' || rel.front() == L'\\') return base + rel.substr(1);
    return base + rel;
}

#pragma pack(push,1)
struct RiffHeader { char id[4]; uint32_t size; char wave[4]; };
struct ChunkHeader { char id[4]; uint32_t size; };
#pragma pack(pop)

static bool readAllFile(const std::wstring& path, std::vector<uint8_t>& out) 
{
    std::filesystem::path p(path);
    if (!std::filesystem::exists(p)) {
        Debug::LogAlways(std::string("[[失敗]] Audioファイルが見つかりませんでした : ") + std::filesystem::absolute(p).string());
        return false;
    }
    std::ifstream ifs(p, std::ios::binary);
    if (!ifs) {
        Debug::LogAlways(std::string("[[失敗]] Audioファイルが開けませんでした : ") + std::filesystem::absolute(p).string());
        return false;
    }

    ifs.seekg(0, std::ios::end);
    size_t sz = static_cast<size_t>(ifs.tellg());
    ifs.seekg(0);
    out.resize(sz);
    ifs.read(reinterpret_cast<char*>(out.data()), sz);
    return ifs.good();
}

std::shared_ptr<AudioClip> AudioManager::loadWavFull(const std::wstring& fullPath, std::string* err) 
{
    std::vector<uint8_t> bin;
    if (!readAllFile(fullPath, bin)) 
    { 
        if (err) {
            *err = "read fail";
            Debug::LogAlways(std::string("Audio load failed: ") + std::filesystem::absolute(fullPath).string());
            return {};
        }

        Debug::LogAlways(std::string("[[失敗]] Audio load : ") + std::filesystem::absolute(fullPath).string()); return {};
    }
    if (bin.size() < sizeof(RiffHeader)) 
    {
        if (err) {
            *err = "too small";
        }
        return {}; 
    }

    auto* rh = reinterpret_cast<const RiffHeader*>(bin.data());
    if (std::strncmp(rh->id, "RIFF", 4)   != 0 || 
        std::strncmp(rh->wave, "WAVE", 4) != 0) {
        if (err) *err = "not WAV"; return {};
    }

    const uint8_t* p = bin.data() + sizeof(RiffHeader);
    const uint8_t* e = bin.data() + bin.size();

    const WAVEFORMATEX* wf = nullptr;
    uint32_t fmtSize = 0;
    const uint8_t* dataPtr = nullptr; uint32_t dataSize = 0;

    while (p + sizeof(ChunkHeader) <= e) {
        auto* ch = reinterpret_cast<const ChunkHeader*>(p);
        p += sizeof(ChunkHeader);
        if (p + ch->size > e) break;

        if (std::strncmp(ch->id, "fmt ", 4) == 0) {
            wf = reinterpret_cast<const WAVEFORMATEX*>(p);
            fmtSize = ch->size;
        }
        else if (std::strncmp(ch->id, "data", 4) == 0) {
            dataPtr = p; dataSize = ch->size;
        }
        size_t adv = ch->size + (ch->size & 1); // even align
        p += adv;
    }

    if (!wf || !dataPtr || !dataSize) { if (err) *err = "fmt/data missing"; return {}; }

    size_t fmtBytes = std::max<size_t>(fmtSize,sizeof(WAVEFORMATEX));
    auto* dup = reinterpret_cast<WAVEFORMATEX*>(new uint8_t[fmtBytes]);
    std::memset(dup,  0, fmtBytes);
    std::memcpy(dup, wf, fmtSize);
    if (fmtSize < sizeof(WAVEFORMATEX)) {
        dup->cbSize = 0;
    }

    auto clip = std::make_shared<AudioClip>();
    clip->wfex = dup;
    clip->pcm.assign(dataPtr, dataPtr + dataSize);
    return clip;
}

std::shared_ptr<AudioClip> AudioManager::LoadWavClip(const std::wstring& path) 
{
    {
        std::lock_guard<std::mutex> lk(mtx_);
        std::wstring fullPath = joinPath(m_baseDir, path);
        auto it = m_PathCache.find(fullPath);
        if (it != m_PathCache.end()) {
            if (auto cached = it->second.lock()) {
                return cached;
            }
        }
    }

    std::vector<uint8_t> bin;
    std::wstring fullPath = joinPath(m_baseDir, path);
    if (!readAllFile(fullPath, bin)) {
        throw std::runtime_error("Audio load failed");
    }
    if (bin.size() < sizeof(RiffHeader)) {
        throw std::runtime_error("WAV too small");
    }

    auto* rh = reinterpret_cast<const RiffHeader*>(bin.data());
    if (std::strncmp(rh->id, "RIFF", 4) != 0 || std::strncmp(rh->wave, "WAVE", 4) != 0) {
        throw std::runtime_error("Not a WAV file");
    }

    const uint8_t* p = bin.data() + sizeof(RiffHeader);
    const uint8_t* e = bin.data() + bin.size();

    const uint8_t* fmtPtr = nullptr;
    uint32_t fmtSize = 0;
    const uint8_t* dataPtr = nullptr;
    uint32_t dataSize = 0;

    while (p + sizeof(ChunkHeader) <= e) 
    {
        auto* ch = reinterpret_cast<const ChunkHeader*>(p);
        p += sizeof(ChunkHeader);
        if (p + ch->size > e) break;

        if (std::strncmp(ch->id, "fmt ", 4) == 0) {
            fmtPtr = p;
            fmtSize = ch->size;
        }
        else if (std::strncmp(ch->id, "data", 4) == 0) {
            dataPtr = p;
            dataSize = ch->size;
        }
        size_t adv = ch->size + (ch->size & 1); // even align
        p += adv;
    }

    if (!fmtPtr || fmtSize < sizeof(PCMWAVEFORMAT) || !dataPtr || dataSize == 0) {
        throw std::runtime_error("fmt/data missing");
    }

    auto* wf = reinterpret_cast<const WAVEFORMATEX*>(fmtPtr);
    WORD formatTag = wf->wFormatTag;
    WORD bits = wf->wBitsPerSample;
    if (formatTag == WAVE_FORMAT_EXTENSIBLE) 
    {
        if (fmtSize < sizeof(WAVEFORMATEXTENSIBLE)) {
            throw std::runtime_error("fmt chunk too small");
        }
        auto* wfx = reinterpret_cast<const WAVEFORMATEXTENSIBLE*>(fmtPtr);
        if (wfx->SubFormat == KSDATAFORMAT_SUBTYPE_PCM) {
            formatTag = WAVE_FORMAT_PCM;
        }
        else if (wfx->SubFormat == KSDATAFORMAT_SUBTYPE_IEEE_FLOAT) {
            formatTag = WAVE_FORMAT_IEEE_FLOAT;
        }
        bits = wfx->Format.wBitsPerSample;
    }

    if (formatTag == WAVE_FORMAT_PCM) 
    {
        if (!(bits == 16 || bits == 24 || bits == 32)) {
            throw std::runtime_error("Unsupported PCM bit depth");
        }
    }
    else if (formatTag == WAVE_FORMAT_IEEE_FLOAT) 
    {
        if (bits != 32) {
            throw std::runtime_error("Unsupported float bit depth");
        }
    }
    else {
        throw std::runtime_error("Unsupported WAV format");
    }

    size_t fmtBytes = std::max<size_t>(fmtSize, sizeof(WAVEFORMATEX));
    auto* dup = reinterpret_cast<WAVEFORMATEX*>(new uint8_t[fmtBytes]);
    std::memset(dup, 0, fmtBytes);
    std::memcpy(dup, fmtPtr, fmtSize);
    if (fmtSize < sizeof(WAVEFORMATEX)) {
        dup->cbSize = 0;
    }

    auto clip = std::make_shared<AudioClip>();
    clip->wfex = dup;
    clip->pcm.assign(dataPtr, dataPtr + dataSize);

    {
        std::lock_guard<std::mutex> lk(mtx_);
        m_PathCache[fullPath] = clip;
    }

    return clip;
}

bool AudioManager::Add(std::string key, std::wstring relativePath) 
{
    if (!m_xa || !m_master) return false; // 先に Init()
    std::string err;
    auto full = joinPath(m_baseDir, relativePath);
    auto clip = loadWavFull(full, &err);

    if (!clip) 
    {
        Debug::LogAlways(std::string("[[失敗]] AudioManager::Add : ") + std::filesystem::absolute(full).string() + " reason=" + err);
        return false;
    }

    std::lock_guard<std::mutex> lk(mtx_);
    m_key2clip[std::move(key)] = std::move(clip);
    return true;
}

bool AudioManager::Add(int id, std::wstring relativePath) 
{
    std::string key = std::string("id:") + std::to_string(id);
    if (!Add(key, std::move(relativePath))) return false;
    std::lock_guard<std::mutex> lk(mtx_);
    m_id2key[id] = key;
    return true;
}

bool AudioManager::Alias(std::string aliasKey, std::string existingKey) 
{
    std::lock_guard<std::mutex> lk(mtx_);
    auto it = m_key2clip.find(existingKey);
    if (it == m_key2clip.end()) return false;
    m_key2clip[std::move(aliasKey)] = it->second;
    return true;
}

bool AudioManager::Alias(int id, std::string existingKey) 
{
    std::lock_guard<std::mutex> lk(mtx_);
    if (!m_key2clip.count(existingKey)) return false;
    m_id2key[id] = std::move(existingKey);
    return true;
}

uint32_t AudioManager::Play(std::string key, const PlayParams& p) 
{
    std::shared_ptr<AudioClip> clip;
    {
        std::lock_guard<std::mutex> lk(mtx_);
        auto it = m_key2clip.find(key);
        if (it == m_key2clip.end()) return 0;
        clip = it->second;
    }

    std::lock_guard<std::mutex> lk2(mtx_);
    uint32_t id = m_nextId++;
    auto inst = std::make_unique<Audio>(m_xa.Get(), m_master, std::move(clip), id);
    inst->Play(p);
    m_instances.emplace_back(std::move(inst));
    return id;
}

uint32_t AudioManager::Play(int id, const PlayParams& p) 
{
    std::string key;
    {
        std::lock_guard<std::mutex> lk(mtx_);
        auto it = m_id2key.find(id);
        if (it == m_id2key.end()) return 0;
        key = it->second;
    }
    return Play(key, p);
}

Audio* AudioManager::find(uint32_t id) 
{
    for (auto& up : m_instances) if (up->Id() == id) return up.get();
    return nullptr;
}

void AudioManager::Stop(uint32_t handle, bool immediate) 
{
    std::lock_guard<std::mutex> lk(mtx_);
    if (auto* p = find(handle)) p->Stop(immediate);
}

void AudioManager::StopAll(bool immediate) 
{
    std::lock_guard<std::mutex> lk(mtx_);
    for (auto& p : m_instances) p->Stop(immediate);
}

void AudioManager::StopAllByName(std::string key, bool immediate) 
{
    std::shared_ptr<AudioClip> clip;
    {
        std::lock_guard<std::mutex> lk(mtx_);
        auto it = m_key2clip.find(key);
        if (it == m_key2clip.end()) return;
        clip = it->second;
    }
    std::lock_guard<std::mutex> lk2(mtx_);
    for (auto& p : m_instances) if (p->Clip() == clip) p->Stop(immediate);
}

void AudioManager::StopAllById(int id, bool immediate) 
{
    std::string key;
    {
        std::lock_guard<std::mutex> lk(mtx_);
        auto it = m_id2key.find(id);
        if (it == m_id2key.end()) return;
        key = it->second;
    }
    StopAllByName(key, immediate);
}

void AudioManager::SetVolume(uint32_t handle, float v) 
{
    std::lock_guard<std::mutex> lk(mtx_);
    if (auto* p = find(handle)) p->SetVolume(v);
}

void AudioManager::SetPitch(uint32_t handle, float r) 
{
    std::lock_guard<std::mutex> lk(mtx_);
    if (auto* p = find(handle)) p->SetPitch(r);
}

void AudioManager::SetPan(uint32_t handle, float pan) 
{
    std::lock_guard<std::mutex> lk(mtx_);
    if (auto* p = find(handle)) p->SetPan(pan);
}

std::shared_ptr<AudioClip> AudioManager::GetClip(std::string_view key) 
{
    std::lock_guard<std::mutex> lk(mtx_);
    auto it = m_key2clip.find(std::string(key));
    return (it == m_key2clip.end()) ? nullptr : it->second;
}

std::shared_ptr<Audio> AudioManager::Create(const AudioConfig& cfg)
{
    if (!m_xa || !m_master) {
        throw std::runtime_error("AudioManager not initialized");
    }

    auto clip = LoadWavClip(cfg.filePath);
    auto audio = std::make_shared<Audio>(m_xa.Get(), m_master, std::move(clip), m_nextId++);
    audio->SetBaseBpm(cfg.baseBpm);
    audio->SetBpm(cfg.bpm);

    PlayParams params = cfg.params;
    if (cfg.loop) {
        //  cfg.loop  true
        //  loopCount
        if (params.loop.loopCount == 0) {
            params.loop.loopCount = XAUDIO2_LOOP_INFINITE;
        }
        // BGM
        params.autoRelease = false;
    }

    if (cfg.autoPlay) {
        audio->Play(params);
    }

    return audio;
}
