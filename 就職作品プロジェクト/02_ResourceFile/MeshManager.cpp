#include "MeshManager.h"
#ifdef _DEBUG
#include <iostream>
#endif


MeshManager::MeshManager()
{
	MeshModel meshModel;
	std::string modelName,filePath,texPath;
	modelName    = "NULL";
	texPath = "01_AssetFile/Model/spot/";
	filePath    = texPath + "spot.fbx";
	std::string tmpStr1(reinterpret_cast<const char*>(filePath.c_str()), filePath.size());
	if (meshModel.Load(filePath, texPath))
	{
		m_Meshes.emplace_back(meshModel);	// ƒ‚ƒfƒ‹‚Ì“o˜^
		m_Names.emplace_back (modelName);	// –¼‘O‚Ì“o˜^
		std::cout << "¬Œ÷ : ƒ‚ƒfƒ‹“o˜^" << std::endl;
		return;
	}


}

void MeshManager::AddMeshModel(std::string modelName, std::string modelPath, std::string texDirectory)
{
	MeshModel meshModel;
	std::string texPath = "01_AssetFile/Model/" ;
	texPath += texDirectory;
	std::string filePath = texPath + modelPath;

	// –¼‘Oƒ`ƒFƒbƒN
	if (IsNameCheck(modelName) == true) 
	{
		std::cout << "Ž¸”s       : modelName”í‚è" << std::endl;
		std::cout << "modelName : " << modelName << std::endl;
		return;
	}

	// ƒ[ƒh
	std::string tmpStr1(reinterpret_cast<const char*>(filePath.c_str()), filePath.size());
	if (meshModel.Load(filePath, texPath))
	{
		m_Meshes.emplace_back(meshModel);	// ƒ‚ƒfƒ‹‚Ì“o˜^
		m_Names .emplace_back(modelName);	// –¼‘O‚Ì“o˜^
		std::cout << "¬Œ÷ : ƒ‚ƒfƒ‹“o˜^" << std::endl; 
		return;
	}

	std::cout << "Ž¸”s : ƒ‚ƒfƒ‹“o˜^" << std::endl;
	return;
}

std::shared_ptr<MeshModel> MeshManager::GetMeshModel(std::string modelName)
{
	
	if (IsNameCheck(modelName) == true) {
		std::cout << "¬Œ÷ : MeshModelŽæ“¾" << modelName << std::endl;
		return m_Meshes[findNumber];
	}
	
	std::cout << "Ž¸”s : GetMeshModel" << modelName << std::endl;
	return m_Meshes[0];
}

bool MeshManager::IsNameCheck(std::string registName)
{
	short roop = 0;
	for (auto name : m_Names)
	{
		if (name == registName) {
			findNumber = roop;
			return true;
		}
		++roop;
	}
	std::cout << "Not Find" << std::endl;

	findNumber = 0;
	return false;
}
