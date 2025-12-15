#pragma once
#include "StaticMesh.h"
#include "ObjectFile/Model.h"
#include <vector>
#include <string>
#include <memory>
#include <unordered_map>

class MeshManager
{
private:

	// 登録名 → MeshInfo のマップ
	std::unordered_map<std::string, MeshInfo> m_MeshMap;
    
    // ベースディレクトリ
	std::string m_FilePath;

	// デフォルト（NULL）モデルの登録名 ("NULL")
	static constexpr const char* m_DefaultName = "NULL";

public:
    MeshManager();
    MeshManager(std::string filePath);
    ~MeshManager() = default;

    // キャッシュをクリア
    void Clear();

    // MeshModel を追加してキャッシュ
    // modelName 登録名
    // modelPath  fbx ファイル名（ベースディレクトリは内部で構築）
    // texDirectory テクスチャフォルダ名（ベースディレクトリは内部で構築）
    // 追加成功なら true（未登録かつロード成功）、重複／失敗なら false
    bool Add(const std::string& modelName,
        const std::string& modelPath,
        const std::string& texDirectory);

    // 登録済みの MeshModel を取得
    // modelName 登録名
    // 成功時は shared_ptr、失敗時はデフォルト登録（NULL）の MeshModel
    std::shared_ptr<StaticMesh> GetStaticMesh(const std::string& modelName);

    // ファイルパスを取得
    std::string GetFilePath(const std::string& modelName);

    // テクスチャディレクトリを取得
    std::string GetTextureDirectory(const std::string& modelName);

};

