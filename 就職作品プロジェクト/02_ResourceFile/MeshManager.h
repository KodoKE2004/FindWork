#pragma once
#include "MeshModel.h"
#include <vector>
#include <string>
#include <memory>


class MeshManager
{
private:

	struct MeshInfo
	{
		std::string					name;
		std::shared_ptr<MeshModel>	mesh;
		std::string					filePath;
		std::string					texDirectory;
	};

	std::vector<MeshInfo> m_MeshInfo;

	short findNumber = 0;

public:
	MeshManager();
	void Clear();

	/// <summary>
	/// Meshの追加
	/// 
	/// モデルのファイル構成
	/// ・fbxとテクスチャが同じフォルダ内に存在している前提
	/// ・texDirectryの中にfbxが存在しないと成り立たない
	/// </summary>
	/// <param name="modelName"></param>名前を登録してモデルを取得
	/// <param name="modelPath"></param>fbxで指定してよいようにする
	/// <param name="texDirectory"></param>パス名は直前まででよい　
	void AddMeshModel(std::string modelName,std::string modelPath,std::string texDirectory);

	std::shared_ptr<MeshModel> GetMeshModel(std::string modelName);

	/// <summary>
	/// 名前のチェック　
	/// 引数の文字列と同じ名前があるか調べる
	/// true = 同名なし false = 同名あり
	/// </summary>
	/// <param name="registName"></param>
	/// <returns></returns>
	bool IsNameCheck(std::string registName);

	std::string GetFilePath(std::string modelName);
	std::string GetTextureDirectory(std::string modelName);

};

