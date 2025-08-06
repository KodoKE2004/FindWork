#pragma once
#include "Shader.h"
#include <unordered_map>
#include <memory>
#include <string>

class ShaderManager
{
private:
	std::unordered_map < std::string, std::shared_ptr<Shader>> m_ShaderList;

public:
	bool Load(std::string shaderName);
	bool AddShader(std::string shaderName, Shader shader);

};

