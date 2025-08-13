#include "ShaderManager.h"
#include "Debug.hpp"

ShaderManager::ShaderManager(std::string entryPoint) : m_EntryPoint(std::move(entryPoint))
{
}
void ShaderManager::AddShader(const std::string& name, ShaderStage stage)
{
    if (HasShader(name)) {
        Debug::Log("ShaderManager: 登録済み '" + name + "' already exists.");
        return;
    }

    // ステージが不明なら名前接頭辞から推定（例: VS_, PS_, GS_, CS_）
    ShaderStage s = stage;
    switch (s) {
    case ShaderStage::VS:
    case ShaderStage::PS:
    case ShaderStage::GS:
    case ShaderStage::CS:
        break; // 既に明示されている
    default:
        if (!InferStageFromName(name, s)) {
            Debug::Log("ShaderManager: could not infer stage from '" + name + "'.");
            return;
        }
        break;
    }

    // ステージに応じて派生クラスを生成
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

    // プロパティ設定 → Create 実行
    shader->SetHlslName(name + "hlsl");
    shader->SetShaderStage(s);

    if (!shader->Create(shader->GetHlslName())) {
        delete shader; // 失敗時は解放
        Debug::Log("ShaderManager: Failed to create shader object for name '" + name + "'.");
        return;
    }

    // 登録
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
		// 命名規則に従っていない場合は生成できない
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

