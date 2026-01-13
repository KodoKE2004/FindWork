
#include "MeshManager.h"
#include "StaticMesh.h"
#include "Debug.hpp"

MeshManager::MeshManager() {
    Debug::Log("[[登録]] MeshManager コンストラクタ");
    // "NULL" モデルを最初に登録しておく
    Add(m_DefaultName, "spot.fbx", "spot");
}

MeshManager::MeshManager(std::string filePath)
{
	m_FilePath = std::move(filePath);
    Debug::Log("[[登録]] MeshManager コンストラクタ");
    // "NULL" モデルを最初に登録しておく
    Add(m_DefaultName, "spot.fbx", "spot");

}

void MeshManager::Clear() {
    m_MeshMap.clear();
}

bool MeshManager::Add(const std::string& modelName,
    const std::string& modelPath,
    const std::string& texDirectory) {
    // 重複チェック
    if (m_MeshMap.find(modelName) != m_MeshMap.end()) {
        Debug::Log("[[失敗]] modelName 被り " + modelName, MessageColor::Red);
        return false;
    }

    // パス構築
    std::string texPath = m_FilePath + texDirectory;
    std::string filePath = texPath + "/" + modelPath;

    // ロード
    auto meshModel = std::make_shared<StaticMesh>();
    if (!meshModel->Load(filePath, texPath)) {
        Debug::Log("[[失敗]] 登録モデル名 " + modelName, MessageColor::Red);
        return false;
    }

    // キャッシュに追加
    MeshInfo info;
    info.mesh = meshModel;
    info.filePath = filePath;
    info.texDirectory = texPath;
    m_MeshMap.emplace(modelName, std::move(info));

    Debug::Log("[[成功]] 登録モデル名 " + modelName,MessageColor::Green);
    return true;
}

std::shared_ptr<StaticMesh> MeshManager::GetStaticMesh(const std::string& modelName) {
    auto it = m_MeshMap.find(modelName);
    if (it != m_MeshMap.end()) {
        Debug::Log("[[成功]] StaticMesh取得 " + modelName, MessageColor::Green);
        return it->second.mesh;
    }
    Debug::Log("[[失敗]] デフォルトを返却  StaticMesh:" + modelName, MessageColor::Red);
    // デフォルト ("NULL") を返す
    return m_MeshMap[m_DefaultName].mesh;
}

std::string MeshManager::GetFilePath(const std::string& modelName) {
    auto it = m_MeshMap.find(modelName);
    if (it != m_MeshMap.end()) {
        Debug::Log("[[成功]] FilePath取得 " + modelName, MessageColor::Green);
        return it->second.filePath;
    }
    Debug::Log("[[失敗]] デフォルトを返却 FilePath : " + modelName, MessageColor::Red);
    return m_MeshMap[m_DefaultName].filePath;
}

std::string MeshManager::GetTextureDirectory(const std::string& modelName) {
    auto it = m_MeshMap.find(modelName);
    if (it != m_MeshMap.end()) {
        Debug::Log("[[成功]] : TextureDirectory取得 " + modelName, MessageColor::Green);
        return it->second.texDirectory;
    }
    Debug::Log("[[失敗]] : TextureDirectory取得, デフォルトを返却 " + modelName, MessageColor::Red);
    return m_MeshMap[m_DefaultName].texDirectory;
}