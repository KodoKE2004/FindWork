#include "TextureManager.h"
#include "Texture.h"
#include "Debug.hpp"

TextureManager::TextureManager(const std::string& basePath)
    : m_FilePath(basePath)
{
}

TextureManager::~TextureManager() = default;

bool TextureManager::HasTexture(const std::string& relativePath) const 
{
    return m_TextureList.find(relativePath) != m_TextureList.end();
}

bool TextureManager::AddTexture(const std::string& relativePath) {
    if (HasTexture(relativePath)) {
        Debug::Log("[[中止]] 取得済みのTexture : " + relativePath);
        return false;
    }

    std::string fullPath = m_FilePath + relativePath;
    auto tex = std::make_shared<Texture>();
    if (!tex->LoadFromFile(fullPath)) {
        return false;
    }

    // Debug::Log("[成功] AddTexture : " + relativePath);
    m_TextureList.emplace(relativePath, tex);
    return true;
}

std::shared_ptr<Texture> TextureManager::GetTexture(const std::string& relativePath) {

    auto it = m_TextureList.find(relativePath);
    if (it != m_TextureList.end()) {
        Debug::Log("[[成功]] GetTexture : " + relativePath);
        return it->second;
    }

    // 未ロードなら AddTexture に委譲
    if (AddTexture(relativePath)) {
        return m_TextureList[relativePath];
    }
    // ロード失敗
    return nullptr;
}