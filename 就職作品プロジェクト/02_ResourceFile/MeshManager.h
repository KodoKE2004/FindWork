#pragma once
#include "MeshModel.h"
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

	/// デフォルト（NULL）モデルの登録名 ("NULL")
	static constexpr const char* m_DefaultName = "NULL";

public:
    MeshManager();
    MeshManager(std::string filePath);
    ~MeshManager() = default;

    /// @brief キャッシュをクリア
    void Clear();

    /// @brief MeshModel を追加してキャッシュ
    /// @param modelName 登録名
    /// @param modelPath  fbx ファイル名（ベースディレクトリは内部で構築）
    /// @param texDirectory テクスチャフォルダ名（ベースディレクトリは内部で構築）
    /// @return 追加成功なら true（未登録かつロード成功）、重複／失敗なら false
    bool AddMeshModel(const std::string& modelName,
        const std::string& modelPath,
        const std::string& texDirectory);

    /// @brief 登録済みの MeshModel を取得
    /// @param modelName 登録名
    /// @return 成功時は shared_ptr、失敗時はデフォルト登録（NULL）の MeshModel
    std::shared_ptr<MeshModel> GetMeshModel(const std::string& modelName);

    /// @brief ファイルパスを取得
    std::string GetFilePath(const std::string& modelName);

    /// @brief テクスチャディレクトリを取得
    std::string GetTextureDirectory(const std::string& modelName);

};

