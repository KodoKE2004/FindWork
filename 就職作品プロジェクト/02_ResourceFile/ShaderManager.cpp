#include "ShaderManager.h"
#include "Debug.hpp"

ShaderManager::ShaderManager(std::string entryPoint) : m_FilePath(std::move(entryPoint))
{
    if (!m_FilePath.empty()) {
        char back = m_FilePath.back();
        if (back != '/' && back != '\\') {
            m_FilePath.push_back('/');
        }
    }
}
void ShaderManager::Add(const std::string& name, ShaderStage stage)
{
    if (HasShader(name)) {
        Debug::Log("ShaderManager: “o˜^Ï‚Ý '" + name + "' already exists.");
        return;
    }

    // ƒXƒe[ƒW‚ª•s–¾‚È‚ç–¼‘OÚ“ªŽ«‚©‚ç„’èi—á: VS_, PS_, GS_, CS_j
    ShaderStage s = stage;
    switch (s) {
    case ShaderStage::VS:
    case ShaderStage::PS:
    case ShaderStage::GS:
    case ShaderStage::CS:
        break; // Šù‚É–¾Ž¦‚³‚ê‚Ä‚¢‚é
    default:
        if (!InferStageFromName(name, s)) {
            Debug::Log("ShaderManager: could not infer stage from '" + name + "'.");
            return;
        }
        break;
    }

    // ƒXƒe[ƒW‚É‰ž‚¶‚Ä”h¶ƒNƒ‰ƒX‚ð¶¬
    BaseShader* shader = nullptr;
    switch (s) {
    case ShaderStage::VS: shader = new VertexShader();  break;
    case ShaderStage::PS: shader = new PixelShader();   break;
    case ShaderStage::GS: shader = new GeometryShader(); break;
    case ShaderStage::CS: shader = new ComputeShader(); break;
    default:
        Debug::Log("ShaderManager: unsupported stage for '" + name + "'.");
        return;
    }

    // ƒvƒƒpƒeƒBÝ’è ¨ Create ŽÀs
    shader->SetHlslName(m_FilePath + name + ".hlsl");
    shader->SetShaderStage(s);

    if (!shader->Create(shader->GetHlslName())) {
        delete shader; // Ž¸”sŽž‚Í‰ð•ú
        Debug::Log("ShaderManager: “o˜^Ž¸”s '" + name + "'.");
        return;
    }

    // “o˜^
    m_ShaderList[name] = shader;
    Debug::Log("ShaderManager: “o˜^Š®—¹ '" + name + "'.");
}

bool ShaderManager::HasShader(const std::string& shaderName) const
{
	return m_ShaderList.find(shaderName) != m_ShaderList.end();
}

bool ShaderManager::InferStageFromName(const std::string& name, ShaderStage& stage) const
{
	if (name.rfind("VS_", 0 && stage == ShaderStage::VS) == 0) { return true; }
	if (name.rfind("PS_", 0 && stage == ShaderStage::PS) == 0) { return true; }
	if (name.rfind("GS_", 0 && stage == ShaderStage::GS) == 0) { return true; }
	if (name.rfind("CS_", 0 && stage == ShaderStage::CS) == 0) { return true; }
	return false;
}

BaseShader* ShaderManager::GetShader(const std::string& name) const
{
	if (auto it = m_ShaderList.find(name); it != m_ShaderList.end())
    {
        return it->second;
	}
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

