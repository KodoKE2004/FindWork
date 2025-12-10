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
            Debug::Log("[[命名違反]] AddShaderを中止します'" + name + "'.");
            return;
        }
        break;
    }

    // ステージに応じて派生クラスを生成
    BaseShader* shader = nullptr;

    switch (s) {
    case ShaderStage::VS: shader = new VertexShader();      break;
    case ShaderStage::PS: shader = new PixelShader();       break;
    case ShaderStage::GS: shader = new GeometryShader();    break;
    case ShaderStage::CS: shader = new ComputeShader();     break;
    default:
        Debug::Log("[[不明なステージ]] AddShaderを中止します'" + name + "'.");
        return;
    }

    // プロパティ設定 → Create 実行
    shader->SetHlslName(m_FilePath + name + ".hlsl");
    shader->SetShaderStage(s);

    if (!shader->Create(shader->GetHlslName())) {
        delete shader; // 失敗時は解放
        Debug::Log("[[失敗]] AddShader'" + name + "'.");
        return;
    }

    // 登録
    m_ShaderList[name] = shader;
    Debug::Log("[[成功]] AddShader '" + name + "'.");
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

bool ShaderManager::ReloadAll()
{
    bool allSuccess = true;

    for (auto& [name, shader] : m_ShaderList)
    {
        if (!shader) { continue; }

        if (!shader->Create(shader->GetHlslName()))
        {
            Debug::Log("[[失敗]] ShaderManagerReload '" + name + "'.\n" );
            allSuccess = false;
        }
        else
        {
            Debug::Log("[[成功]] ShaderManagerReloaded '" + name + "'.\n");
        }
    }

    return allSuccess;
}

