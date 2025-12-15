#pragma once
#include "Texture.h"
#include <unordered_map>
#include <memory>
#include <string>

class TextureManager
{
private:
	std::unordered_map<std::string, std::shared_ptr<Texture>> m_TextureList;	
	short m_RegistNumber = 0;

	std::string m_FilePath;

public:
    // basePath テクスチャファイルのベースディレクトリ
    explicit TextureManager(const std::string& basePath);
    ~TextureManager();

    // 指定パスのテクスチャを取得（未ロードならロードしてキャッシュ）
    // relativePath ベースパスからの相対パス
    // 読み込み成功なら Texture*、失敗なら nullptr
    std::shared_ptr<Texture> GetTexture(const std::string& relativePath);

    // 明示的にテクスチャをロードしてキャッシュに追加
    // relativePath ベースパスからの相対パス
    // 追加成功なら true（未ロードかつロード成功）、既にロード済み／失敗なら false
    bool AddTexture(const std::string& relativePath);

    // テクスチャがキャッシュ済みかをチェック
    // relativePath ベースパスからの相対パス
    // ロード済みなら true
    bool HasTexture(const std::string& relativePath) const;
};

