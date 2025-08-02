#include "MeshManager.h"
#include <memory>
#ifdef _DEBUG
#include <iostream>
#endif


MeshManager::MeshManager()
{
	auto meshModel = std::make_shared<MeshModel>();
	std::string modelName,filePath,texDirectory;
	modelName    = "NULL";
	texDirectory = "01_AssetFile/Model/spot/";
	filePath    = texDirectory + "spot.fbx";
	std::string tmpStr1(reinterpret_cast<const char*>(filePath.c_str()), filePath.size());
	if (meshModel->Load(filePath, texDirectory))
	{
		MeshInfo info;
		info.name = modelName;
		info.mesh = std::make_shared<MeshModel>();
		info.mesh = meshModel;
		info.filePath     = filePath;
		info.texDirectory = texDirectory;

		m_MeshInfo.emplace_back(info);	// ’Ç‰Á
		std::cout << "¬Œ÷ : ƒ‚ƒfƒ‹NULL“o˜^Š®—¹" << std::endl;
		return;
	}


}

void MeshManager::AddMeshModel(std::string modelName, std::string modelPath, std::string texDirectory)
{
	auto meshModel = std::make_shared<MeshModel>();
	std::string texPath = "01_AssetFile/Model" ;
	texPath += "/" + texDirectory;
	std::string filePath = texPath + "/" + modelPath;

	// –¼‘Oƒ`ƒFƒbƒN
	if (IsNameCheck(modelName) == true) 
	{
		std::cout << "Ž¸”s : modelName”í‚è" << modelName  << std::endl;
		return;
	}

	// ƒ[ƒh
	std::string tmpStr1(reinterpret_cast<const char*>(filePath.c_str()), filePath.size());
	if (meshModel->Load(filePath, texPath))
	{
		MeshInfo info;
		
		info.name = modelName;
		info.mesh = std::make_shared<MeshModel>();
		info.mesh = meshModel;
		info.filePath	  = filePath;
		info.texDirectory = texPath;
		
		m_MeshInfo.emplace_back(info);

		std::cout << "¬Œ÷ : ƒ‚ƒfƒ‹“o˜^ " << modelName<< std::endl; 
		return;
	}

	std::cout << "Ž¸”s : ƒ‚ƒfƒ‹“o˜^" << modelName << std::endl;
	return;
}

std::shared_ptr<MeshModel> MeshManager::GetMeshModel(std::string modelName)
{
	
	if (IsNameCheck(modelName) == true) {
		std::cout << "¬Œ÷ : MeshModelŽæ“¾" << modelName << std::endl;
		return m_MeshInfo[findNumber].mesh;
	}
	
	std::cout << "Ž¸”s : GetMeshModel : " << modelName << std::endl;
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
	std::cout << "–¼‘O”í‚è‚È‚µ : " << registName << std::endl;

	findNumber = 0;
	return false;
}

std::string MeshManager::GetFilePath(std::string modelName)
{
	if (IsNameCheck(modelName) == true) {
		std::cout << "¬Œ÷ : FilePathŽæ“¾" << modelName << std::endl;
		return m_MeshInfo[findNumber].filePath;
	}

	std::cout << "Ž¸”s : FilePathŽæ“¾ : " << modelName << std::endl;
	return m_MeshInfo[0].filePath;

}

std::string MeshManager::GetTextureDirectory(std::string modelName)
{
	if (IsNameCheck(modelName) == true) {
		std::cout << "¬Œ÷ : TextureDirectoryŽæ“¾" << modelName << std::endl;
		return m_MeshInfo[findNumber].texDirectory;
	}

	std::cout << "Ž¸”s : TextureDirectoryŽæ“¾ : " << modelName << std::endl;
	return m_MeshInfo[0].texDirectory;

}
