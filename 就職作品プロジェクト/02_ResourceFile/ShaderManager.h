#pragma once
#include "Shader.h"
#include "Renderer.h"
#include <unordered_map>
#include <memory>
#include <string>


class ShaderManager
{
private:
	// シェーダーの名前とポインタを紐づけるためのマップ
	std::unordered_map<std::string, BaseShader*> m_ShaderList;
	std::string m_FilePath; // エントリーポイント名

public:
	
	ShaderManager() = default;
	ShaderManager(std::string entryPoint);
	~ShaderManager()
	{
		for (auto& [name, shader] : m_ShaderList)
		{
			delete shader; // BaseShaderのポインタを解放
		}
		m_ShaderList.clear();
	}

	void Add(const std::string& name, ShaderStage stage);	// シェーダーを登録
	bool HasShader(const std::string& shaderName) const;				// 登録済みの名前検索
	bool InferStageFromName(const std::string& name, ShaderStage& stage) const;


	void SetEntryPoint(std::string entryPoint) { m_FilePath = std::move(entryPoint); }

	// リストにあるシェーダーの名前を検索して取得
	BaseShader* GetShader(const std::string& name) const;
	BaseShader* GetOrCreateShader(const std::string& name);

	bool ReloadAll();

	//BaseShader* Aho(const std::string& name)
	//{
	//	//// すでに登録されていたら
	//	//if (auto it = m_ShaderList.find(name); it != m_ShaderList.end())
	//	//{
	//	//	return it->second;
	//	//}

	//	//BaseShader* shader;

	//	//// 見つからなかった場合の戻り値は std::string::npos
	//	//if (size_t pos = name.find("VS_"); pos == 0)
	//	//{
	//	//	shader = new VertexShader(name);
	//	//} 
	//	//else if (size_t pos = name.find("PS_"); pos == 0)
	//	//{
	//	//	shader = new PixelShader(name);
	//	//}

	//	//if (!shader->Create())
	//	//{
	//	//	delete shader;
	//	//	return nullptr;
	//	//}

	//	//return list.at(name) = shader;

	//	// ちなみに

	//	// こうなってるけど
	//	//bool ShaderManager::HasShader(const std::string & shaderName) const
	//	//{
	//	//	return m_ShaderList.find(shaderName) != m_ShaderList.end();
	//	//}

	//	// これでいけるで？
	//	//return m_ShaderList.contains(shaderName);
	//}


};

