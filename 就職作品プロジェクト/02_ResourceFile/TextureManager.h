#pragma once
#include "Texture.h"
#include <unordered_map>
#include <memory>
#include <string>

class TextureManager
{
private:
	std::unordered_map<std::string, std::unique_ptr<Texture>> m_TextureList;	
	short m_RegistNumber = 0;

	std::string m_FilePath;

public:
    /// @param basePath テクスチャファイルのベースディレクトリ
    explicit TextureManager(const std::string& basePath);
    ~TextureManager();

    /// @brief 指定パスのテクスチャを取得（未ロードならロードしてキャッシュ）
    /// @param relativePath ベースパスからの相対パス
    /// @return 読み込み成功なら Texture*、失敗なら nullptr
    Texture* GetTexture(const std::string& relativePath);

    /// @brief 明示的にテクスチャをロードしてキャッシュに追加
    /// @param relativePath ベースパスからの相対パス
    /// @return 追加成功なら true（未ロードかつロード成功）、既にロード済み／失敗なら false
    bool AddTexture(const std::string& relativePath);

    /// @brief テクスチャがキャッシュ済みかをチェック
    /// @param relativePath ベースパスからの相対パス
    /// @return ロード済みなら true
    bool HasTexture(const std::string& relativePath) const;
};

