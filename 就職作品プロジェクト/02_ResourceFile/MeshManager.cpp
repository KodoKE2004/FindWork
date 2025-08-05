#include "MeshManager.h"
#include <memory>
#include <iostream>
#include "Debug.hpp"

MeshManager::MeshManager()
{
	Debug::Log ("コンストラクタ：NULLモデル登録　開始");
	AddMeshModel("NULL","spot.fbx","spot");
}

void MeshManager::Clear()
{
	m_MeshInfo.clear();
}

void MeshManager::AddMeshModel(std::string modelName, std::string modelPath, std::string texDirectory)
{
	auto meshModel = std::make_shared<MeshModel>();
	std::string texPath = "01_AssetFile/Model" ;
	texPath += "/" + texDirectory;
	std::string filePath = texPath + "/" + modelPath;

	// 名前チェック
	if (IsNameCheck(modelName) == true) 
	{
		Debug::Log ("失敗 : modelName被り " + modelName);
		return;
	}

	// ロード
	std::string tmpStr1(reinterpret_cast<const char*>(filePath.c_str()), filePath.size());
	if (meshModel->Load(filePath, texPath))
	{
		MeshInfo info;
		
		info.name = modelName;
		info.mesh = meshModel;
		info.filePath	  = filePath;
		info.texDirectory = texPath;
		
		m_MeshInfo.emplace_back(info);

		Debug::Log("成功 : モデル登録 " + modelName);
		return;
	}

	Debug::Log("失敗 : モデル登録" + modelName);
	return;
}

std::shared_ptr<MeshModel> MeshManager::GetMeshModel(std::string modelName)
{
	
	if (IsNameCheck(modelName) == true) {
		Debug::Log("成功 : MeshModel取得" + modelName);
		return m_MeshInfo[findNumber].mesh;
	}
	
	Debug::Log("失敗 : GetMeshModel : " + modelName);
	return m_MeshInfo[0].mesh;
}

bool MeshManager::IsNameCheck(std::string registName)
{
	short roop = 0;
	for (auto name : m_MeshInfo)
	{
		if (name.name == registName) {
			findNumber = roop;
			return true;
		}
		++roop;
	}
	Debug::Log("名前被りなし : " + registName);

	findNumber = 0;
	return false;
}

std::string MeshManager::GetFilePath(std::string modelName)
{
	if (IsNameCheck(modelName) == true) {
		Debug::Log("成功 : FilePath取得" + modelName);
		return m_MeshInfo[findNumber].filePath;
	}

	Debug::Log("失敗 : FilePath取得 : " + modelName);
	return m_MeshInfo[0].filePath;

}

std::string MeshManager::GetTextureDirectory(std::string modelName)
{
	if (IsNameCheck(modelName) == true) {
		Debug::Log("成功 : TextureDirectory取得" + modelName);
		return m_MeshInfo[findNumber].texDirectory;
	}

	Debug::Log("失敗 : TextureDirectory取得 : " + modelName);
	return m_MeshInfo[0].texDirectory;

}
