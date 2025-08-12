#include "ShaderManager.h"
#include "Debug.hpp"

ShaderManager::ShaderManager(std::string entryPoint) : m_EntryPoint(std::move(entryPoint))
{
}
void ShaderManager::AddShader(const std::string& name, ShaderStage stage)
{
	if (HasShader(name)) {
		Debug::Log("ShaderManager: “o˜^Ï‚Ý '" + name + "' already exists.");
		return;
	}
	
	// ‚±‚±‚ÅBaseShader‚Ìƒ|ƒCƒ“ƒ^‚ð¶¬
	BaseShader* shader;
	shader->SetHlslName(name);		
	shader->SetShaderStage(stage);	
	shader = shader->TryCreateShaderFromName();

	if(!shader) {
		Debug::Log("ShaderManager: Failed to create shader for name '" + name + "'.");
		return;
	}

	if (shader->Create(name) == false) {
		delete shader; // Ž¸”s‚µ‚½ê‡‚Íƒƒ‚ƒŠ‚ð‰ð•ú
		Debug::Log("ShaderManager: Failed to create shader object for name '" + name + "'.");
		return;
	}
	m_ShaderList[name] = shader;
	Debug::Log("ShaderManager: Registered shader '" + name + "'.");
}

bool ShaderManager::HasShader(const std::string& shaderName) const
{
	return m_ShaderList.find(shaderName) != m_ShaderList.end();
}

bool ShaderManager::InferStageFromName(const std::string& name, ShaderStage& stage) const
{
	if (name.rfind("VS_", 0) == 0) { stage = ShaderStage::VS; return true; }
	if (name.rfind("PS_", 0) == 0) { stage = ShaderStage::PS; return true; }
	if (name.rfind("GS_", 0) == 0) { stage = ShaderStage::GS; return true; }
	if (name.rfind("CS_", 0) == 0) { stage = ShaderStage::CS; return true; }
	return false;
}

BaseShader* ShaderManager::GetShader(const std::string& name) const
{
	if (auto it = m_ShaderList.find(name); it != m_ShaderList.end()) return it->second;
	return nullptr;
}

BaseShader* ShaderManager::GetOrCreateShader(const std::string& name)
{
	if (auto* s = GetShader(name)) return s;

	ShaderStage stage{};
	if (!InferStageFromName(name, stage))
	{
		// –½–¼‹K‘¥‚É]‚Á‚Ä‚¢‚È‚¢ê‡‚Í¶¬‚Å‚«‚È‚¢
		return nullptr;
	}

	BaseShader* shader = nullptr;
	switch (stage)
	{
	case ShaderStage::VS: shader = new VertexShader();   break;
	case ShaderStage::PS: shader = new PixelShader();    break;
	case ShaderStage::GS: shader = new GeometryShader(); break;
	case ShaderStage::CS: shader = new ComputeShader();  break;
	default: return nullptr;
	}

	if (!shader->Create(name))
	{
		delete shader;
		return nullptr;
	}

	m_ShaderList.emplace(name, shader);
	return shader;
}

