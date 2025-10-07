#pragma once
#include "Shader.h"
#include "Renderer.h"
#include <unordered_map>
#include <memory>
#include <string>

//// Shaderってなまえつかわれてたから
//class BaseClass
//{
//	friend class ShaderManager;
//protected:
//	std::string name;
//	BaseClass(std::string name) : name(name) {}
//
//	virtual bool Create() = 0;
//	virtual void SetGPU() = 0;
//
//public:
//	std::string GetName() const { return name; }
//};
//
//class VertexShader : public BaseClass
//{
//	friend class ShaderManager;
//private:
//	ID3D11VertexShader* ptr = nullptr;
//	ID3D11InputLayout* il = nullptr;	
//
//	VertexShader(std::string name) : BaseClass(name) {}
//
//	bool Create()
//	{
//		// 頂点データの定義
//		D3D11_INPUT_ELEMENT_DESC layout[] =
//		{
//			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,		0,	D3D11_APPEND_ALIGNED_ELEMENT,	D3D11_INPUT_PER_VERTEX_DATA, 0 },
//			{ "NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT,		0,	D3D11_APPEND_ALIGNED_ELEMENT,	D3D11_INPUT_PER_VERTEX_DATA, 0 },
//			{ "COLOR",    0, DXGI_FORMAT_R32G32B32A32_FLOAT,	0,	D3D11_APPEND_ALIGNED_ELEMENT,	D3D11_INPUT_PER_VERTEX_DATA, 0 },
//			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,			0,	D3D11_APPEND_ALIGNED_ELEMENT,   D3D11_INPUT_PER_VERTEX_DATA, 0 }
//		};
//
//		unsigned int numElements = ARRAYSIZE(layout);
//
//		ID3D11Device* device = Renderer::GetDevice();
//
//		// 頂点シェーダーオブジェクトを生成、同時に頂点レイアウトも生成
//		bool sts = CreateVertexShader(device,
//			name.c_str(),
//			"main",
//			"vs_5_0",
//			layout,
//			numElements,
//			&ptr,
//			&il);
//
//		if (!sts) {
//			MessageBox(nullptr, "CreateVertexShader error", "error", MB_OK);
//			return false;
//		}
//
//		return true;
//	}
//
//public:
//	void SetGPU() override
//	{
//		auto dc = Renderer::GetDeviceContext();
//		dc->VSSetShader(ptr, nullptr, 0);
//		dc->IASetInputLayout(il);
//	}
//};
//
//class PixelShader : public BaseClass
//{
//	friend class ShaderManager;
//private:
//	ID3D11PixelShader* ptr;
//
//	PixelShader(std::string name) : BaseClass(name) {}
//
//	bool Create()
//	{
//		auto device = Renderer::GetDevice();
//
//		// ピクセルシェーダーを生成
//		bool sts = CreatePixelShader(			// ピクセルシェーダーオブジェクトを生成
//			device,							// デバイスオブジェクト
//			name.c_str(),
//			"main",
//			"ps_5_0",
//			&ptr);
//
//		if (!sts) {
//			MessageBox(nullptr, "CreatePixelShader error", "error", MB_OK);
//			return false;
//		}
//
//		return true;
//	}
//
//	void SetGPU() override
//	{
//		auto dc = Renderer::GetDeviceContext();
//		dc->PSSetShader(ptr, nullptr, 0);
//	}
//
//};

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

