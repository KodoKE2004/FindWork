#pragma once
#include "Shader.h"
#include <unordered_map>
#include <memory>
#include <string>

class ShaderManager
{
private:
	std::unordered_map < std::string, std::shared_ptr<Shader>> m_ShaderList;
	std::string m_FilePath;
public:
	ShaderManager(std::string filePath);

	/// @brief PS,VS両方とも作成
	/// @param shaderName シェーダーの名前
	/// @return 
	bool Load(const std::string& shaderName);

	/// @brief VSのみ作成をする
	/// @param shaderName 
	/// @return 
	bool LoadVertexSahder(const std::string& shaderName);
	
	/// @brief PSのみ作成する
	/// @param shaderName 
	/// @return 
	bool LoadPixelSahder (const std::string& shaderName);
	
	bool AddShader(const std::string& shaderName, const Shader& shader);
	Shader* GetShader(const std::string& shaderName);
	bool HasShader(const std::string& shaderName) const;

};

